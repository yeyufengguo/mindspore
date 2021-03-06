/**
 * Copyright 2019 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MINDSPORE_CCSRC_PARALLEL_OPS_INFO_MATMUL_INFO_H_
#define MINDSPORE_CCSRC_PARALLEL_OPS_INFO_MATMUL_INFO_H_

#include <string>
#include <list>
#include <unordered_map>
#include <vector>
#include <memory>
#include "ir/value.h"
#include "parallel/ops_info/operator_info.h"
#include "parallel/strategy.h"
#include "parallel/auto_parallel/operator_costmodel.h"
#include "common/utils.h"

namespace mindspore {
namespace parallel {
class MatMulBase : public OperatorInfo {
 public:
  MatMulBase(const std::string& name, const Shapes& inputs_shape, const Shapes& outputs_shape,
             const PrimitiveAttrs& attrs)
      : OperatorInfo(name, inputs_shape, outputs_shape, attrs) {
    matmulcost_ptr = std::make_shared<MatMulCost>();
  }
  ~MatMulBase() override = default;

  Status Init(const StrategyPtr& strategy) override;
  Status InitForCostModel(const StrategyPtr& strategy) override;

  // Generate all strategies and the corresponding cost for this MatMul operator
  Status GenerateStrategies(int32_t stage_id) override;
  Status SetCostUnderStrategy(const StrategyPtr& strategy) override;
  Status PrepareStrategy(int32_t stage_id, size_t dev_num, Dimensions combined_partitions, size_t input0_shape_size,
                         size_t input1_shape_size, StrategyPtr* sp);

  OperatorCostPtr GetOperatorCost() const override { return matmulcost_ptr; }
  Status SwapLastTwoElements(Shape* shape);

 protected:
  Status InferMirrorOps() override;
  Status InferForwardCommunication() override;
  Status InferTensorInfo() override;
  Status InferDevMatrixShape() override;
  Status InferTensorMap() override;
  Status InferTensorLayout(TensorLayouts* inputs_layout, TensorLayouts* outputs_layout);
  void InitTensorInfoForCost(std::vector<TensorInfo>*);
  Status CheckForTensorSliceValid() const;
  Status GetAttrs() override;

  bool transpose_a_ = false;
  bool transpose_b_ = false;
  size_t mat_a_dimension_ = 0;
  size_t mat_b_dimension_ = 0;

  MatMulCostPtr matmulcost_ptr;
};

class MatMul : public MatMulBase {
 public:
  MatMul(const std::string& name, const Shapes& inputs_shape, const Shapes& outputs_shape, const PrimitiveAttrs& attrs)
      : MatMulBase(name, inputs_shape, outputs_shape, attrs) {}
  ~MatMul() override = default;

 protected:
  Status CheckStrategy(const StrategyPtr& strategy) override;
};

class MatMulInfo : public MatMul {
 public:
  MatMulInfo(const std::string& name, const Shapes& inputs_shape, const Shapes& outputs_shape,
             const PrimitiveAttrs& attrs)
      : MatMul(name, inputs_shape, outputs_shape, attrs) {}
  ~MatMulInfo() override = default;
};
}  // namespace parallel
}  // namespace mindspore
#endif  // MINDSPORE_CCSRC_PARALLEL_OPS_INFO_MATMUL_INFO_H_
