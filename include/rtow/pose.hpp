#pragma once

#include <cmath>
#include <optional>

#include "rtow/matrix_utils.hpp"
#include "rtow/vec_utils.hpp"

namespace rtow {

template <typename T = float>
using pose = Vec<T, 6>;

//////////////////////////////////////////////////////////////////////
// For coordinate conventions, see:
// Aircraft control and simulation By Brian L. Stevens, Frank L. Lewis says
// p,q,r are standard symbols for roll, pitch and yaw.
//
// This is rotation about the fixed frame. see eqn 2.20 of Modelling and
// Control of Robot Manipulators.
// roll  about x axis, sinze +x is forward
// pitch about y axis, since +y is right
// yaw   about z axis, sinze +z is down
//
//
// Composite rotation is ZYX order:
//   R = Rz*Ry*Rx
//
// Note that all angles are in radians!
//
//////////////////////////////////////////////////////////////////////

// Returns ZYX Euler angles for a rotation matrix in radians.
template <typename T>
Vec3<T> EulerZYX(const Mat3<T>& R) {
  using std::abs;
  Vec3<T> rpy_rad;

  // Protect against domain error when both atan2 args are 0, which happens
  // when pitch=+/-90
  if (abs(R(2, 1)) > T(1e-9) || abs(R(2, 2)) > T(1e-9)) {
    rpy_rad[0] = atan2(R(2, 1), R(2, 2));
  } else {
    rpy_rad[0] = T(0);
  }

  // Need to protect against roundoff error around +/-1, which will
  // result in NaN.
  rpy_rad[1] = R(2, 0) >= T(1.0) ? -T(M_PI_2) : R(2, 0) <= T(-1.0) ? T(M_PI_2) : asin(-R(2, 0));

  // Protect against domain error when both atan2 args are 0, which happens
  // when pitch is +/- 90.
  if (abs(R(1, 0)) > T(1e-9) || abs(R(0, 0)) > T(1e-9)) {
    rpy_rad[2] = atan2(R(1, 0), R(0, 0));
  } else {
    // force roll to be 0 since roll and yaw are dependent
    rpy_rad[0] = T(0);
    rpy_rad[2] = atan2(R(1, 2) / -R(2, 0), R(0, 2) / -R(2, 0));
  }

  return rpy_rad;
}

/// Convert from a list of scalars to matrix form.
template <typename T>
Mat4<T> Pose2T(const T x, const T y, const T z, const T p, const T q, const T r) {
  const T cr = cos(p);  // 'roll' (rotation about x in NED)
  const T sr = sin(p);
  const T cp = cos(q);  // 'pitch' (rotation about y in NED)
  const T sp = sin(q);
  const T cy = cos(r);  // 'yaw' (rotation about z in NED)
  const T sy = sin(r);

  // clang-format off
  Mat4<T> m = {{cy*cp, -sy*cr+cy*sp*sr,  sy*sr+cy*sp*cr,    x,
                sy*cp,  cy*cr+sy*sp*sr, -cy*sr+sy*sp*cr,    y,
                  -sp,           cp*sr,           cp*cr,    z,
                 T(0),            T(0),            T(0),   T(1)}};
  // clang-format on
  return m;
}

/// Convert from vector to matrix form.
template <typename T>
Mat4<T> Pose2T(const pose<T>& xyzpqr) {
  return Pose2T(xyzpqr[0], xyzpqr[1], xyzpqr[2], xyzpqr[3], xyzpqr[4], xyzpqr[5]);
}

/// Convert from vector to matrix form.
template <typename T>
Mat4<T> Pose2T(const T* xyzpqr) {
  return Pose2T(xyzpqr[0], xyzpqr[1], xyzpqr[2], xyzpqr[3], xyzpqr[4], xyzpqr[5]);
}

/// Convert from matrix to vector form.
template <typename T>
pose<T> T2Pose(const Mat4<T>& Tab) {
  const Mat3<T> R =
      // clang-format off
  {{
    Tab(0, 0), Tab(0, 1), Tab(0, 2),
    Tab(1, 0), Tab(1, 1), Tab(1, 2),
    Tab(2, 0), Tab(2, 1), Tab(2, 2) 
  }};
  // clang-format on

  const auto pqr = EulerZYX(R);
  return {{Tab(0, 3), Tab(1, 3), Tab(2, 3), pqr[0], pqr[1], pqr[2]}};
}

/// Convert from a list of scalars to a rotation matrix.
template <typename T>
Mat3<T> Pose2R(const T p, const T q, const T r) {
  const T cr = cos(p);  // 'roll' (rotation about x in NED)
  const T sr = sin(p);
  const T cp = cos(q);  // 'pitch' (rotation about y in NED)
  const T sp = sin(q);
  const T cy = cos(r);  // 'yaw' (rotation about z in NED)
  const T sy = sin(r);

  // clang-format off
  Mat3<T> R = {{ cy*cp, -sy*cr+cy*sp*sr,  sy*sr+cy*sp*cr,
                 sy*cp,  cy*cr+sy*sp*sr, -cy*sr+sy*sp*cr,
                   -sp,           cp*sr,           cp*cr }};
  // clang-format on
  return R;
}

/// Convert from a list of scalars to a rotation matrix.
template <typename T>
Mat3<T> Pose2R(const pose<T>& xyzpqr) {
  return Pose2R(xyzpqr[3], xyzpqr[4], xyzpqr[5]);
}

template <typename T>
Vec3<T> position(const pose<T>& pose) {
  return {pose[0], pose[1], pose[2]};
}

template <typename T>
Vec3<T> attitude(const pose<T>& pose) {
  return {pose[3], pose[4], pose[5]};
}

/// Invert a transformation
template <typename T>
pose<T> TInv(const pose<T>& tab) {
  const Mat3<T> R = Pose2R(tab);
  const Mat3<T> RT = transpose(R);
  const Vec3<T> pqr = EulerZYX(RT);
  const Vec3<T> xyz = -RT * position(tab);
  return {xyz[0], xyz[1], xyz[2], pqr[0], pqr[1], pqr[2]};
}

/// Transform a point by a transformation.
template <typename T>
Vec3<T> Transform(const Mat4<T>& Twa, const Vec3<T>& xap) {
  const Vec<T, 4> p(xap[0], xap[1], xap[2], T(1));
  const Vec<T, 4> res = Twa * p;
  return Vec3<T>(res[0], res[1], res[2]);
}

/// Covenience function to compound a vector into a point.
template <typename T>
Vec3<T> Transform(const pose<T>& tab, const Vec3<T>& xap) {
  return Pose2R(tab) * xap + position(tab);
}

template <typename T>
Vec3d TComp(const pose<T>& tab, const Vec3<T>& pt) {
  return (Pose2R(tab) * pt) + position(tab);
}

/// Covenience function to compound two pose vectors.
template <typename T>
pose<T> TComp(const pose<T>& tab, const pose<T>& tbc) {
  const Mat4<T> Tab = Pose2T(tab);
  const Mat4<T> Tbc = Pose2T(tbc);
  const Mat4<T> Tac = Tab * Tbc;
  return T2Pose(Tac);
}

template <typename T>
Vec3<T> TComp(const Mat4<T>& Twa, const Vec3<T>& pt) {
  const Vec<T, 4> p{{pt[0], pt[1], pt[2], T(1)}};
  const Vec<T, 4> res = Twa * p;
  return {{res[0], res[1], res[2]}};
}

/// Covenience function to compound a matrix and a pose vector
template <typename T>
pose<T> TComp(const Mat4<T>& Tab, const pose<T>& tbc) {
  const Mat4<T> Tbc = Pose2T(tbc);
  const Mat4<T> Tac = Tab * Tbc;
  return T2Pose(Tac);
}

/// Covenience function to compound two pose matrices.
template <typename T>
pose<T> TComp(const Mat4<T>& Tab, const Mat4<T>& Tbc) {
  const Mat4<T> Tac = Tab * Tbc;
  return T2Pose(Tac);
}

/// Convenience function to compound the inverse of one transform with another.  Useful
/// when computing relative poses between two transformations expressed in the same frame.
template <typename T>
pose<T> TInvComp(const pose<T>& tab, const pose<T>& tac) {
  return TComp(TInv(tab), tac);
}

/// Transform a direction vector (i.e. rotation only) by R.
template <typename T>
Vec3<T> TransformDir(const Mat3<T>& R, const Vec3<T>& xap) {
  return R * xap;
}

/// Transform a direction vector (i.e. rotation only) by the rotation of Tab.
template <typename T>
Vec3<T> TransformDir(const Mat4<T>& Tab, const Vec3<T>& xap) {
  return Pose2R(T2Pose(Tab)) * xap;
}

/// Transform a direction vector (i.e. rotation only) by the rotation of tab.
template <typename T>
Vec3<T> TransformDir(const pose<T>& tab, const Vec3<T>& xap) {
  return Pose2R(tab) * xap;
}

// Transform a point by the inverse of T.
template <typename T>
Vec3<T> InvTransform(const pose<T>& tab, const Vec3<T>& p) {
  return transpose(Pose2R(tab)) * (p - position(tab));
}

// Transform a point by the inverse of T.
template <typename T>
Vec3<T> InvTransform(const Mat4<T>& Tab, const Vec3<T>& p) {
  return InvTransform(T2Pose(Tab), p);
}

// Check if this pose has been marked as invalid by setting the X component of
// the translation to a signaling NaN.
template <typename T>
bool IsPoseValid(const pose<T>& tab) {
  return !std::isnan(tab[0]);
}

/// Delta between 2 poses defined in a common reference-frame 'o'
/// \param[in] pose_oa 6-dof pose of 'a' in 'o'
/// \param[in] pose_ob 6-dof pose of 'b' in 'o'
/// \return[out] pose_delta pose_oa [-] pose_ob
template <typename T = float>
inline pose<T> PoseDelta(const pose<T>& pose_oa, const pose<T>& pose_ob) {
  return TComp(TInv(pose_oa), pose_ob);
}

template <typename T = float>
inline pose<T> InvalidPose() {
  return pose<T>::constant(std::numeric_limits<double>::quiet_NaN());
}

template <typename T>
// note: all vectors in world frame
pose<T> LookAt(const Vec3<T>& position, const Vec3<T>& at, const Vec3<T>& up) {
  const Vec3<T> z = normalize(at - position);
  const Vec3<T> x = normalize(cross(up, z));
  const Vec3<T> y = normalize(cross(z, x));

  const Mat4<T> Tab =
      // clang-format off
  {{
    x[0], y[0], z[0], position.x(), 
    x[1], y[1], z[1], position.y(),
    x[2], y[2], z[2], position.z(),
    T(0), T(0), T(0), T(1)
  }};
  // clang-format on

  return T2Pose(Tab);
}

}  // namespace rtow
