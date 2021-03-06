#include "kalman_filter.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;

// Please note that the Eigen library does not initialize
// VectorXd or MatrixXd objects with zeros upon creation.

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  /**
  TODO:
    * predict the state
  */
  x_ = F_ * x_;
  MatrixXd F_trans = F_.transpose();
  P_ = F_ * P_ * F_trans + Q_;
}

void KalmanFilter::Update(const VectorXd &z) {
  /**
  TODO:
    * update the state by using Kalman Filter equations
  */
  VectorXd y_ = z - H_ * x_;

  MatrixXd H_trans = H_.transpose();
  MatrixXd S_ = H_ * P_ * H_trans + R_;
  MatrixXd S_inv = S_.inverse();
  MatrixXd K_ = P_ * H_trans * S_inv;

  x_ = x_ + (K_ * y_);
  int x_size = x_.size();
  //MatrixXd I_ = MatrixXd::Identity(x_size, x_size);
  P_ -= K_ * H_ * P_;
}

void NormalizeAngle(double& phi) {
  phi = atan2(sin(phi), cos(phi));
}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  /**
  TODO:
    * update the state by using Extended Kalman Filter equations
  */
  float rho = sqrt(x_(0) * x_(0) + x_(1) * x_(1));
  float phi = atan2(x_(1), x_(0));
  if (x_(1) == 0 and x_(0) == 0) {
    phi = 0.000001;
  }
  float rho_dot;
  // avoid division by zero
  if (fabs(rho) < 0.0001) {
    rho_dot = 0;
  } else {
    rho_dot = (x_(0) * x_(2) + x_(1) * x_(3)) / rho;
  }

  VectorXd z_prime(3);
  z_prime << rho, phi, rho_dot;
  VectorXd y_ = z - z_prime;
  NormalizeAngle(y_(1));

  MatrixXd H_trans = H_.transpose();
  MatrixXd S_ = H_ * P_ * H_trans + R_;
  MatrixXd S_inv = S_.inverse();
  MatrixXd K_ = P_ * H_trans * S_inv;

  x_ = x_ + (K_ * y_);
  int x_size = x_.size();
  P_ -= K_ * H_ * P_;
}
