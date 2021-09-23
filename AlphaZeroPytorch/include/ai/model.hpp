// TorchTestCMake.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <game\game.hpp>
#include "memory.hpp"
#include <tuple>

namespace AlphaZero {
	namespace ai {
		class ResNet : public torch::nn::Module {
		public: torch::nn::Conv2d conv1, conv2;
		private: torch::nn::BatchNorm2d batch;
		private: torch::nn::LeakyReLU activ;
		private: int kernel1, kernel2;

		public: ResNet(int inp, int out, int kernelsize1, int kernelsize2);
		public: torch::Tensor forward(torch::Tensor);
		};

		class Value_head : torch::nn::Module {
		public: torch::nn::Conv2d conv;
		public: torch::nn::Linear lin1, lin2;
		private: torch::nn::ReLU relu;
		private: torch::nn::Tanh tanh;
		private: int size;

		public: Value_head(int inp, int hidden_size, int out, int kernels);
		public: torch::Tensor forward(torch::Tensor);
		};

		class Policy_head : torch::nn::Module {
		public: torch::nn::Conv2d conv;
		public: torch::nn::Linear lin1;
		private: torch::nn::Softmax softmax;
		private: int size;

		public: Policy_head(int inp, int hidden, int out);
		public: torch::Tensor forward(torch::Tensor);
		};

		typedef torch::nn::MSELoss Loss;
		typedef torch::optim::SGD Optimizer;
		typedef torch::optim::SGDOptions OptimizerOptions;
		
		class Model : torch::nn::Module {
		//private: torch::nn::Conv2d headLayer;
		private: ResNet res1, res2;
		private: Value_head value_head;
		private: Policy_head policy_head;

		private: Loss loss;
		private: Optimizer optim;
			
		public: Model();
		public: std::pair<torch::Tensor, torch::Tensor> forward(torch::Tensor);
		public: std::pair<float, float> train(const std::pair<torch::Tensor, torch::Tensor>&x, const std::pair<torch::Tensor, torch::Tensor>&y);

		public: std::pair<float, torch::Tensor>predict(std::shared_ptr<Game::GameState> state);
		public: static std::tuple<torch::Tensor, torch::Tensor, torch::Tensor> getBatch(std::shared_ptr<Memory> memory, unsigned int batchSize);
		public: void fit(const std::tuple<torch::Tensor, torch::Tensor, torch::Tensor>& batch, const unsigned short& run, const unsigned short& trainingLoop);

		private: ResNet register_custom_module(ResNet& net, std::string layer);
		private: Value_head register_custom_module(Value_head& net);
		private: Policy_head register_custom_module(Policy_head& net);

		};
	}
}
// customizable section
#define modelTest false

inline AlphaZero::ai::Model::Model() :
	//headLayer(register_module("conv", torch::nn::Conv2d()))
	res1(this->register_custom_module(ResNet(2, 75, 5, 5), "Residual_1")),
	res2(this->register_custom_module(ResNet(75, 75, 5, 5), "Residual_2")),
	value_head(this->register_custom_module(Value_head(75, 420, 210, 10))),
	policy_head(this->register_custom_module(Policy_head(75, 84, 42))),
	optim(Optimizer(this->parameters(), OptimizerOptions(0.0001).momentum(0.9)))
{
}

inline std::pair<torch::Tensor, torch::Tensor> AlphaZero::ai::Model::forward(torch::Tensor x)
{
	x = this->res1.forward(x);
	x = this->res2.forward(x);

	// compute individual heads
	torch::Tensor value = this->value_head.forward(x);
	torch::Tensor poly = this->policy_head.forward(x);
	return { value, poly };
};
// end of cutimizable section

inline AlphaZero::ai::ResNet::ResNet(int inp, int out, int kernelsize1, int kernelsize2) :
	kernel1(kernelsize1), kernel2(kernelsize2),
	conv1(torch::nn::Conv2d(torch::nn::Conv2dOptions(inp, out, kernelsize1))),
	conv2(torch::nn::Conv2d(torch::nn::Conv2dOptions(out, out, kernelsize2))),
	batch(torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(out))),
	activ(torch::nn::LeakyReLU(torch::nn::LeakyReLU()))
{
}

inline torch::Tensor AlphaZero::ai::ResNet::forward(torch::Tensor x)
{
#if modelTest
	std::cout << x.sizes() << std::endl;
#endif 
	x = torch::nn::functional::pad(x, torch::nn::functional::PadFuncOptions({ kernel1 / 2, kernel1 / 2, kernel1 / 2, kernel1 / 2 }));
	x = this->conv1(x);
	torch::Tensor y = torch::nn::functional::pad(x, torch::nn::functional::PadFuncOptions({ kernel2 / 2, kernel2 / 2, kernel2 / 2, kernel2 / 2 }));
	y = this->conv2(y);
	y = this->batch(y);
	return x + y;
}

inline AlphaZero::ai::Value_head::Value_head(int inp, int hidden_size, int out, int convOut) :
	conv(torch::nn::Conv2d(torch::nn::Conv2dOptions(inp, convOut, 1))),
	lin1(torch::nn::Linear(torch::nn::LinearOptions(hidden_size, out))),
	lin2(torch::nn::Linear(torch::nn::LinearOptions(out, 1))),
	relu(torch::nn::ReLU()), tanh(torch::nn::Tanh())
{
	this->size = hidden_size;
}

inline torch::Tensor AlphaZero::ai::Value_head::forward(torch::Tensor x)
{
#if modelTest
	std::cout << "value" << std::endl;
	std::cout << x.sizes() << std::endl;
#endif 
	x = this->conv(x);
	x = this->lin1(x.reshape({ x.size(0), this->size }));
	x = this->relu(x);
	x = this->lin2(x);
	x = this->tanh(x);
	return x;
}

inline AlphaZero::ai::Policy_head::Policy_head(int inp, int hidden, int out) :
	conv(torch::nn::Conv2d(torch::nn::Conv2dOptions(inp, 2, 1))),
	lin1(torch::nn::Linear(hidden, out)),
	softmax(torch::nn::Softmax(torch::nn::SoftmaxOptions(1)))
{
	this->size = hidden;
}

inline torch::Tensor AlphaZero::ai::Policy_head::forward(torch::Tensor x)
{
#if modelTest
	std::cout << "poly" << std::endl;
	std::cout << x.sizes() << std::endl;
#endif 
	x = this->conv(x);
	x = this->lin1(x.reshape({ x.size(0), this->size }));
	return this->softmax(x);
}

inline std::pair<float, float> AlphaZero::ai::Model::train(const std::pair<torch::Tensor, torch::Tensor>& x, const std::pair<torch::Tensor, torch::Tensor>& y)
{
	auto valLoss = torch::mse_loss(x.first, y.first);
	auto plyLoss = torch::mse_loss(x.second, y.second);
	torch::autograd::backward({ valLoss, plyLoss });
	this->optim.step();
	return { torch::mean(valLoss).item().toFloat(), torch::mean(plyLoss).item().toFloat() };
}

inline std::pair<float, torch::Tensor> AlphaZero::ai::Model::predict(std::shared_ptr<Game::GameState> state)
{
	torch::Tensor NNInput = state->toTensor();
	std::pair<torch::Tensor, torch::Tensor> NNOut = this->forward(NNInput);
	float value = NNOut.first[0].item<float>();
	return {value, NNOut.second };
}

inline std::tuple<torch::Tensor, torch::Tensor, torch::Tensor> AlphaZero::ai::Model::getBatch(std::shared_ptr<Memory> memory, unsigned int batchSize)
{
	std::tuple<torch::Tensor, torch::Tensor, torch::Tensor> output =
	{
		at::zeros({batchSize, input_snape_z, input_shape_y, input_shape_x}),
		at::zeros({batchSize, action_count}),
		at::zeros({batchSize, 1})
	};
	for (unsigned short idx = 0; idx < batchSize; idx++) {
		auto state = memory->getState();
		state->state->toTensor(std::get<0>(output), idx);
		std::get<1>(output)[idx] = at::from_blob(state->av.data(), { action_count });
		std::get<2>(output)[idx] = torch::tensor({ state->value });
	}
	return output;
}

inline void AlphaZero::ai::Model::fit(const std::tuple<torch::Tensor, torch::Tensor, torch::Tensor>& batch, const unsigned short& run, const unsigned short& trainingLoop)
{
	std::cout << std::get<0>(batch).sizes() << std::endl;
	std::pair<torch::Tensor, torch::Tensor> trueVals = this->forward(std::get<0>(batch));
	std::pair<float, float> error = this->train(trueVals, { std::get<1>(batch), std::get<2>(batch) });
#if ModelLogger
	debug::log::modelLogger->info("model error in iteration {} on batch {} had valueError of {} and polyError of {}", run, trainingLoop, std::get<0>(error), std::get<1>(error));
#endif
}

inline AlphaZero::ai::ResNet AlphaZero::ai::Model::register_custom_module(ResNet& net, std::string layer)
{
	register_module(layer + "_conv1", net.conv1);
	register_module(layer + "_conv2", net.conv2);
	return net;
}
inline AlphaZero::ai::Value_head AlphaZero::ai::Model::register_custom_module(Value_head& net)
{
	register_module("value_conv", net.conv);
	register_module("value_lin1", net.lin1);
	register_module("value_lin2", net.lin2);
	return net;
}
inline AlphaZero::ai::Policy_head AlphaZero::ai::Model::register_custom_module(Policy_head& net)
{
	register_module("policy_conv", net.conv);
	register_module("policy_linear", net.lin1);
	return net;
}
// TODO: Reference additional headers your program requires here.
