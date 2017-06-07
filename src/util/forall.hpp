#ifndef CHAI_forall_HPP
#define CHAI_forall_HPP

#include "chai/config.hpp"
#include "chai/ExecutionSpaces.hpp"
#include "chai/ArrayManager.hpp"

struct sequential {};
#if defined(ENABLE_CUDA)
struct cuda {};
#endif

template <typename LOOP_BODY>
void forall_kernel_cpu(int begin, int end, LOOP_BODY body)
{
  for (int i = 0; i < (end - begin); ++i) {
    body(i);
  }
}

/*
 * \brief Run forall kernel on CPU.
 */
template <typename LOOP_BODY>
void forall(sequential, int begin, int end, LOOP_BODY body) {
  chai::ArrayManager* rm = chai::ArrayManager::getInstance();

  rm->setExecutionSpace(chai::CPU);

  forall_kernel_cpu(begin, end, body);

  rm->setExecutionSpace(chai::NONE);
}

#if defined(ENABLE_CUDA)
template <typename LOOP_BODY>
__global__ void forall_kernel_gpu(int start, int length, LOOP_BODY body) {
  int idx = blockDim.x * blockIdx.x + threadIdx.x;

  if (idx < length) {
    body(idx);
  }
}

/*
 * \brief Run forall kernel on GPU.
 */
template <typename LOOP_BODY>
void forall(cuda, int begin, int end, LOOP_BODY&& body) {
  chai::ArrayManager* rm = chai::ArrayManager::getInstance();

  rm->setExecutionSpace(chai::GPU);

  size_t blockSize = 32;
  size_t gridSize = (end - begin + blockSize - 1)/blockSize;

  forall_kernel_gpu<<<gridSize, blockSize>>>(begin, end-begin, body);

  rm->setExecutionSpace(chai::NONE);
}
#endif

#endif // CHAI_forall_HPP
