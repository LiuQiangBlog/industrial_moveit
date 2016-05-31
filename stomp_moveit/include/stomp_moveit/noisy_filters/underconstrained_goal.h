/*
 * UNDERCONSTRAINED_GOAL.h
 *
 *  Created on: Apr 21, 2016
 *      Author: Jorge Nicho
 */

#ifndef INDUSTRIAL_MOVEIT_STOMP_MOVEIT_SRC_FILTERS_UNDERCONSTRAINED_GOAL_H_
#define INDUSTRIAL_MOVEIT_STOMP_MOVEIT_SRC_FILTERS_UNDERCONSTRAINED_GOAL_H_

#include <moveit/robot_model/robot_model.h>
#include <array>
#include <Eigen/Core>
#include <boost/random.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <stomp_moveit/noisy_filters/stomp_noisy_filter.h>

namespace stomp_moveit
{
namespace noisy_filters
{

class UnderconstrainedGoal : public StompNoisyFilter
{
public:
  UnderconstrainedGoal();
  virtual ~UnderconstrainedGoal();

  virtual bool initialize(moveit::core::RobotModelConstPtr robot_model_ptr,
                          const std::string& group_name,const XmlRpc::XmlRpcValue& config);

  virtual bool configure(const XmlRpc::XmlRpcValue& config);

  virtual bool setMotionPlanRequest(const planning_scene::PlanningSceneConstPtr& planning_scene,
                   const moveit_msgs::MotionPlanRequest &req,
                   const stomp_core::StompConfiguration &config,
                   moveit_msgs::MoveItErrorCodes& error_code);

  /**
   * @brief filters the parameters and modifies the original values
   *
   * @param start_timestep    start index into the 'parameters' array, usually 0.
   * @param num_timesteps     number of elements to use from 'parameters' starting from 'start_timestep'
   * @param iteration_number  The current iteration count in the optimization loop
   * @param rollout_number    index of the noisy trajectory whose cost is being evaluated.
   * @param parameters [num_dimensions] x [num_timesteps]
   * @return false if no filtering was applied
   */
  virtual bool filter(std::size_t start_timestep,
                      std::size_t num_timesteps,
                      int iteration_number,
                      int rollout_number,
                      Eigen::MatrixXd& parameters,
                      bool& filtered);

  /**
   * @brief Called by the Stomp at the end of the optimization process
   *
   * @param success           Whether the optimization succeeded
   * @param total_iterations  Number of iterations used
   * @param final_cost        The cost value after optimizing.
   */
  virtual void done(bool success,int total_iterations,double final_cost){}


  virtual std::string getName() const
  {
    return name_ + "/" + group_name_;
  }

  virtual std::string getGroupName() const
  {
    return group_name_;
  }

protected:

  bool runIK(const Eigen::Affine3d& tool_goal_pose,const Eigen::VectorXd& init_joint_pose,
                   Eigen::VectorXd& joint_pose);

protected:

  std::string name_;
  std::string group_name_;

  // tool goal
  Eigen::Affine3d tool_goal_pose_;

  // ik
  Eigen::ArrayXd joint_update_rates_;
  Eigen::ArrayXi dof_nullity_;
  Eigen::ArrayXd cartesian_convergence_thresholds_;
  double update_weight_;
  int max_iterations_;
  Eigen::VectorXd ref_goal_joint_pose_;

  // robot
  moveit::core::RobotModelConstPtr robot_model_;
  moveit::core::RobotStatePtr state_;
  std::string tool_link_;

};

} /* namespace filters */
} /* namespace stomp_moveit */

#endif /* INDUSTRIAL_MOVEIT_STOMP_MOVEIT_SRC_FILTERS_UNDERCONSTRAINED_GOAL_H_ */
