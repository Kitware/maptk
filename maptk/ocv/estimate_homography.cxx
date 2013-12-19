/*ckwg +5
 * Copyright 2013 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */


#include <maptk/ocv/estimate_homography.h>
#include <maptk/ocv/matrix.h>
#include <boost/foreach.hpp>
#include <opencv2/calib3d/calib3d.hpp>

namespace maptk
{

namespace ocv
{


/// Estimate a homography matrix from corresponding points
matrix_3x3d
estimate_homography
::estimate(const std::vector<vector_2d>& pts1,
           const std::vector<vector_2d>& pts2,
           std::vector<bool>& inliers,
           double inlier_scale) const
{

  std::vector<cv::Point2f> points1, points2;
  BOOST_FOREACH(const vector_2d& v, pts1)
  {
    points1.push_back(cv::Point2f(v.x(), v.y()));
  }
  BOOST_FOREACH(const vector_2d& v, pts2)
  {
    points2.push_back(cv::Point2f(v.x(), v.y()));
  }

  cv::Mat inliers_mat;
  cv::Mat H = cv::findHomography( cv::Mat(points1), cv::Mat(points2),
                                 CV_RANSAC,
                                 inlier_scale,
                                 inliers_mat );
  inliers.resize(inliers_mat.rows);
  inliers_mat.copyTo(inliers);

  return matrix_from_ocv<3,3,double>(H);
}


} // end namespace ocv

} // end namespace maptk
