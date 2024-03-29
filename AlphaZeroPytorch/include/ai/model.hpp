﻿// TorchTestCMake.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <game/game.hpp>
#include "memory.hpp"
#include <string>
#include <tuple>
#include <jce/string.hpp>
#include <string>
#include <cmath>
#include <jce/save.hpp>
#include <jce/load.hpp>
#include <test/testUtils.hpp>


namespace AlphaZero {
	namespace ai {
    // top of the neural networks model stack consisting of a Conv2d, batchNorm and relu layer
		class TopLayer : public torch::nn::Module {
		public: torch::nn::Conv2d conv1;
		public: torch::nn::LayerNorm batch;
		public: torch::nn::LeakyReLU relu;
		private: int kernel1;

		public: TopLayer(int inp, int out, int kernelsize1);
      // evaluate layer
		public: torch::Tensor forward(torch::Tensor);
      // move to specific device
		public: void moveTo(c10::Device device);
		};

    // residual block as consisting of 2 conv2d, 2 batchnorms, and leaky relu
		class ResNet : public torch::nn::Module {
		public: torch::nn::Conv2d conv1, conv2;
		public: torch::nn::LayerNorm batch, batch2;
		public: torch::nn::LeakyReLU activ;
		private: int kernel1, kernel2;

		public: ResNet(int inp, int out, int kernelsize1, int kernelsize2);
      // evaluate layer
		public: torch::Tensor forward(torch::Tensor);
      // move to specific device
		public: void moveTo(c10::Device device);
		};

    // value head see paper for details
		class Value_head : torch::nn::Module {
		private: bool isSecondRun = false;
		private: torch::Tensor tmpX;

		public: torch::nn::Conv2d conv;
		public: torch::nn::Linear lin1, lin2;
		public: torch::nn::LeakyReLU relu;
		public: torch::nn::Tanh tanh;
		private: int size;

		public: Value_head(int inp, int hidden_size, int out, int kernels);
		public: torch::Tensor forward(torch::Tensor);
		public: void moveTo(c10::Device device);
		};

    // policy head, see paper for details
		class Policy_head : torch::nn::Module {
		public: torch::nn::Conv2d conv;
		public: torch::nn::Linear lin1;
		public: torch::nn::LeakyReLU relu;
		private: int size;

		public: Policy_head(int inp, int hidden, int out);
		public: torch::Tensor forward(torch::Tensor);
		public: void moveTo(c10::Device device);
		};

		typedef torch::nn::MSELoss Loss;
		typedef torch::optim::SGD Optimizer;
		typedef torch::optim::SGDOptions OptimizerOptions;

    // fully assembled model and some utils functions see paper for nn structure
		class Model : public torch::nn::Module {
		private: TopLayer top;
		private: ResNet res1, res2, res3, res4, res5, res6;
		private: Value_head value_head;
		private: Policy_head policy_head;

		private: const char* device;

		private: Loss loss;
		private: Optimizer optim;

		public: Model(const char* device);
		public: std::pair<torch::Tensor, torch::Tensor> forward(torch::Tensor);

      // perform one training step
		public: std::pair<float, float> train(const std::pair<torch::Tensor, torch::Tensor>& x, const std::pair<torch::Tensor, torch::Tensor>& y);

      // predict model state for MCTS
		public: std::pair<float, torch::Tensor>predict(std::shared_ptr<Game::GameState> state);
      // get batch from model
		public: static std::tuple<torch::Tensor, torch::Tensor, torch::Tensor> getBatch(std::shared_ptr<Memory> memory, unsigned int batchSize);
      // fit batch with training loops and epochs
    public: void fit(const std::tuple<torch::Tensor, torch::Tensor, torch::Tensor>& batch, const unsigned short& run, const unsigned short& trainingLoop);

      // functions to save the model
		public: void save_version(unsigned int version);
		public: void save_as_current();
		public: void save_to_file(char* filename);
		public: void jce_save_current(char* filename);

      // functions to load the model
		public: void load_version(unsigned int version);
		public: void load_current();
		public: void load_from_file(char* filename);
		public: void jce_load_from_file(char* filename);

      // functions to copy the model and to move to other device
		public: void copyModel(Model*);
		private: void copyParameters(torch::OrderedDict<std::string, torch::Tensor> prams);
		public: void moveTo(c10::Device device);

      // custom layer resigtering methods.
		private: TopLayer register_custom_module(TopLayer net);
		private: ResNet register_custom_module(ResNet net, std::string layer);
		private: Value_head register_custom_module(Value_head net);
		private: Policy_head register_custom_module(Policy_head net);

		};
	}
}
// customizable section
#define modelTest false
#define randomModel false
#define convSize 5

inline AlphaZero::ai::Model::Model(const char* _device) :
	top(this->register_custom_module(TopLayer(2, 75, convSize))),
	res1(this->register_custom_module(ResNet(75, 75, convSize, convSize), "Residual_1")),
	res2(this->register_custom_module(ResNet(75, 75, convSize, convSize), "Residual_2")),
	res3(this->register_custom_module(ResNet(75, 75, convSize, convSize), "Residual_3")),
	res4(this->register_custom_module(ResNet(75, 75, convSize, convSize), "Residual_4")),
	res5(this->register_custom_module(ResNet(75, 75, convSize, convSize), "Residual_5")),
	res6(this->register_custom_module(ResNet(75, 75, convSize, convSize), "Residual_6")),
	value_head(this->register_custom_module(Value_head(75, 420, 210, 10))),
	policy_head(this->register_custom_module(Policy_head(75, 84, 42))),
	optim(Optimizer(this->parameters(), OptimizerOptions(learningRage).momentum(Momentum))),
	device(_device)
{
	this->moveTo(c10::Device(_device));
}

inline std::pair<torch::Tensor, torch::Tensor> AlphaZero::ai::Model::forward(torch::Tensor x)
{
#if randomModel
	return { torch::rand({x.size(0), 1}), torch::rand({x.size(0), action_count}) };
#else
	x = this->top.forward(x);
	x = this->res1.forward(x);
	x = this->res2.forward(x);
	x = this->res3.forward(x);
	x = this->res4.forward(x);
	x = this->res5.forward(x);
	x = this->res6.forward(x);

	// compute individual heads
	torch::Tensor value = this->value_head.forward(x.clone());
	torch::Tensor poly = this->policy_head.forward(x.clone());

	return { value, poly };
#endif
};
// end of cutimizable section

inline AlphaZero::ai::TopLayer::TopLayer(int inp, int out, int kernelsize1) :
	conv1(this->register_module("conv1", torch::nn::Conv2d(torch::nn::Conv2dOptions(inp, out, kernelsize1)))),
	batch(this->register_module("batch", torch::nn::LayerNorm(torch::nn::LayerNormOptions({ out, input_shape_y, input_shape_x })))),
	relu(this->register_module("ReLU", torch::nn::LeakyReLU(torch::nn::LeakyReLU()))),
	kernel1(kernelsize1 / 2)
{
}
inline torch::Tensor AlphaZero::ai::TopLayer::forward(torch::Tensor x)
{
	x = torch::nn::functional::pad(x, torch::nn::functional::PadFuncOptions({ kernel1, kernel1, kernel1, kernel1 }));
	x = this->conv1(x);
	x = this->batch(x);
	x = this->relu(x);
	return x;
}

inline void AlphaZero::ai::TopLayer::moveTo(c10::Device device)
{
	this->conv1->to(device, true);
	this->batch->to(device, true);
	this->relu->to(device, true);
}

inline AlphaZero::ai::ResNet::ResNet(int inp, int out, int kernelsize1, int kernelsize2) :
	kernel1(kernelsize1), kernel2(kernelsize2),
	conv1(this->register_module("conv1", torch::nn::Conv2d(torch::nn::Conv2dOptions(inp, out, kernelsize1)))),
	conv2(this->register_module("conv2", torch::nn::Conv2d(torch::nn::Conv2dOptions(out, out, kernelsize2)))),
	batch(this->register_module("batch1", torch::nn::LayerNorm(torch::nn::LayerNormOptions({ out, input_shape_y, input_shape_x })))),
	batch2(this->register_module("batch2", torch::nn::LayerNorm(torch::nn::LayerNormOptions({ out, input_shape_y, input_shape_x })))),
	activ(this->register_module("activ", torch::nn::LeakyReLU(torch::nn::LeakyReLU())))
{
	if (torch::cuda::is_available()) {
		this->moveTo(c10::Device("cuda:0"));
	}
}

inline torch::Tensor AlphaZero::ai::ResNet::forward(torch::Tensor x)
{
#if modelTest
	std::cout << x.sizes() << std::endl;
#endif 
	auto y = x.clone();
	x = torch::nn::functional::pad(x, torch::nn::functional::PadFuncOptions({ kernel1 / 2, kernel1 / 2, kernel1 / 2, kernel1 / 2 }));
	x = this->conv1(x);
	x = this->batch(x);
	x = this->activ(x);

	x = torch::nn::functional::pad(x, torch::nn::functional::PadFuncOptions({ kernel2 / 2, kernel2 / 2, kernel2 / 2, kernel2 / 2 }));
	x = this->conv2(x);
	x = this->batch2(x);
	return this->activ(x + y);
}

inline void AlphaZero::ai::ResNet::moveTo(c10::Device device)
{
	this->conv1->to(device, true);
	this->conv2->to(device, true);
	this->batch->to(device, true);
	this->batch2->to(device, true);
	this->activ->to(device, true);
}

inline AlphaZero::ai::Value_head::Value_head(int inp, int hidden_size, int out, int convOut) :
	conv(this->register_module("conv", torch::nn::Conv2d(torch::nn::Conv2dOptions(inp, convOut, 1)))),
	lin1(this->register_module("lin1", torch::nn::Linear(torch::nn::LinearOptions(hidden_size, out)))),
	lin2(this->register_module("lin2", torch::nn::Linear(torch::nn::LinearOptions(out, 1)))),
	relu(this->register_module("relu", torch::nn::LeakyReLU())), 
	tanh(this->register_module("tanh", torch::nn::Tanh()))
{
	this->size = hidden_size;

	if (torch::cuda::is_available())
	{
		this->moveTo(c10::Device("cuda:0"));
	}
}

inline torch::Tensor AlphaZero::ai::Value_head::forward(torch::Tensor x)
{
#if modelTest
	std::cout << "value" << std::endl;
	std::cout << x.sizes() << std::endl;
#endif 
	x = this->conv(x);
	x = this->relu(x);
	x = this->lin1(x.reshape({ x.size(0), this->size }));
	x = this->relu(x);
	x = this->lin2(x);
	x = this->tanh(x);
	return x;
}

inline void AlphaZero::ai::Value_head::moveTo(c10::Device device)
{
	this->conv->to(device, true);
	this->lin1->to(device, true);
	this->relu->to(device, true);
	this->lin2->to(device, true);
	this->tanh->to(device, true);
}

inline AlphaZero::ai::Policy_head::Policy_head(int inp, int hidden, int out) :
	conv(this->register_module("conv", torch::nn::Conv2d(torch::nn::Conv2dOptions(inp, 2, 1)))),
	lin1(this->register_module("lin1", torch::nn::Linear(hidden, out))),
	relu(this->register_module("relu", torch::nn::LeakyReLU()))
{
	this->size = hidden;

	if (torch::cuda::is_available()) {
		this->moveTo(c10::Device("cuda:0"));
	}
}

inline torch::Tensor AlphaZero::ai::Policy_head::forward(torch::Tensor x)
{
#if modelTest
	std::cout << "poly" << std::endl;
	std::cout << x.sizes() << std::endl;
#endif 
	x = this->conv(x);
	x = this->relu(x);
	x = this->lin1(x.reshape({ x.size(0), this->size }));
	return x;
}

inline void AlphaZero::ai::Policy_head::moveTo(c10::Device device)
{
	this->conv->to(device, true);
	this->lin1->to(device, true);
}

inline torch::Tensor polyLoss(torch::Tensor a, torch::Tensor b)
{
#if true
	auto c = torch::where(b == 0, a, b);
	return torch::mse_loss(a, c);
#else
	return torch::mse_loss(a, b);
#endif
}

inline std::pair<float, float> AlphaZero::ai::Model::train(const std::pair<torch::Tensor, torch::Tensor>& x, const std::pair<torch::Tensor, torch::Tensor>& y)
{
	//std::cout << x.first << std::endl << y.first << std::endl;

	auto valLoss = 0.5f * torch::mse_loss(x.first, y.first);
	auto plyLoss = 0.5f * polyLoss(x.second, y.second);
	auto loss = (valLoss + plyLoss);

	loss.backward();
	this->optim.step();
	this->optim.zero_grad();
	std::pair<float, float> error = { torch::mean(valLoss).item().toFloat(), torch::mean(plyLoss).item().toFloat()};

	if (std::isnan(error.first))
	{
		//std::cout << valLoss << std::endl << plyLoss << std::endl;
		std::cout << x.first  << std::endl << y.first  << std::endl;
		std::cout << x.second << std::endl << y.second << std::endl;
		return error;
	}

	return error;
}

inline std::pair<float, torch::Tensor> AlphaZero::ai::Model::predict(std::shared_ptr<Game::GameState> state)
{
	torch::Tensor NNInput = state->toTensor().to(c10::Device(this->device));
	std::pair<torch::Tensor, torch::Tensor> NNOut = this->forward(NNInput);
	float value = NNOut.first[0].item<float>();
	return { value, NNOut.second };
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
		std::get<1>(output)[idx] = at::from_blob(state->av.data(), { action_count }).toType(torch::kFloat16);
		std::get<2>(output)[idx] = torch::tensor({ state->value });
	}
	return output;
}

inline void AlphaZero::ai::Model::fit(const std::tuple<torch::Tensor, torch::Tensor, torch::Tensor>& batch, const unsigned short& run, const unsigned short& trainingLoop)
{
	std::pair<torch::Tensor, torch::Tensor> NNVals = this->forward(std::get<0>(batch).to(c10::Device(this->device)));
	std::pair<float, float> error = this->train(NNVals, 
		{ 
		std::get<2>(batch).to(c10::Device(this->device)), 
		std::get<1>(batch).to(c10::Device(this->device))
		});
#if ModelLogger
	debug::log::modelLogger->info("model error in iteration {} on batch {} had valueError of {} and polyError of {}", run, trainingLoop, std::get<0>(error), std::get<1>(error));
#endif
#if LossLogger
	debug::log::_lossLogger.addValue(error);
#endif
}

inline void AlphaZero::ai::Model::save_version(unsigned int version)
{
	char buffer[50];
	std::sprintf(buffer, "models/run_%d/V_%d.torch", runVersion, version);
	std::cout << buffer << std::endl;
	this->save_to_file(buffer);
}

inline void AlphaZero::ai::Model::save_as_current()
{
	char buffer[50];
	std::sprintf(buffer, "models/run_%d/currentModel.torch", runVersion);
	this->save_to_file(buffer);
}

inline void AlphaZero::ai::Model::save_to_file(char* filename)
{
	torch::serialize::OutputArchive out;
	this->save(out);
	std::string model_path = std::string(filename);
	out.save_to(model_path);
}

inline void AlphaZero::ai::Model::jce_save_current(char* filename)
{
	std::ofstream out(filename, std::ios::binary);
	jce::save(out, this->named_parameters(true));
	out.close();
}


inline void AlphaZero::ai::Model::load_version(unsigned int version)
{
	std::cout << "loading ...";
	char buffer[50];
	std::sprintf(buffer, "models/run_%d/V_%d.torch", runVersion, version);
	this->load_from_file(buffer);
	std::cout << " loaded Version " << version << std::endl;
}

inline void AlphaZero::ai::Model::load_current()
{
	char buffer[50];
	std::sprintf(buffer, "models/run_%d/currentModel.torch", runVersion);
	this->load_from_file(buffer);
}

inline void AlphaZero::ai::Model::load_from_file(char* filename)
{
	torch::serialize::InputArchive inp;
	std::string model_path = std::string(filename);
	inp.load_from(model_path);
	this->load(inp);
}

inline void AlphaZero::ai::Model::jce_load_from_file(char* filename)
{
	std::cout << "loading ...\t";
	torch::autograd::GradMode::set_enabled(false);
	torch::OrderedDict<std::string, torch::Tensor> map;
	std::ifstream in(filename, std::ios::binary);
	if (in.is_open())
	{
		jce::load(in, map);
		this->copyParameters(map);
	}
	test::printSuccess(in.is_open());
	in.close();
	torch::autograd::GradMode::set_enabled(true);
}

inline void AlphaZero::ai::Model::copyModel(AlphaZero::ai::Model* model)
{
	torch::autograd::GradMode::set_enabled(false);
	auto new_params = model->named_parameters(true);
	this->copyParameters(new_params);
	torch::autograd::GradMode::set_enabled(true);
}

inline void AlphaZero::ai::Model::copyParameters(torch::OrderedDict<std::string, torch::Tensor> new_params)
{
	auto params = this->named_parameters(true);
	auto buffers = this->named_buffers(true);
	for (auto& val : new_params) {
		auto name = val.key();
		auto* t = params.find(name);
		if (t != nullptr) {
			t->copy_(val.value());
		}
		else {
			t = buffers.find(name);
			if (t != nullptr) {
				t->copy_(val.value());
			}
		}
	}
}

inline void AlphaZero::ai::Model::moveTo(c10::Device device)
{
	this->top.moveTo(device);

	this->res1.moveTo(device);
	this->res2.moveTo(device);
	this->res3.moveTo(device);
	this->res4.moveTo(device);
	this->res5.moveTo(device);
	this->res6.moveTo(device);

	this->value_head.moveTo(device);
	this->policy_head.moveTo(device);
}

inline AlphaZero::ai::TopLayer AlphaZero::ai::Model::register_custom_module(TopLayer net)
{
	register_module("TopLayer_conv", net.conv1);
	register_module("TopLayer_batch", net.batch);
	register_module("TopLayer_ReLU", net.relu);
	return net;
}

inline AlphaZero::ai::ResNet AlphaZero::ai::Model::register_custom_module(ResNet net, std::string layer)
{
	register_module(layer + "_conv1", net.conv1);
	register_module(layer + "_conv2", net.conv2);
	register_module(layer + "_batch1", net.batch);
	register_module(layer + "_batch2", net.batch2);
	register_module(layer + "_active", net.activ);
	return net;
}
inline AlphaZero::ai::Value_head AlphaZero::ai::Model::register_custom_module(Value_head net)
{
	register_module("value_conv", net.conv);
	register_module("value_lin1", net.lin1);
	register_module("value_lin2", net.lin2);
	register_module("value_ReLU", net.relu);
	register_module("value_tanh", net.tanh);
	return net;
}
inline AlphaZero::ai::Policy_head AlphaZero::ai::Model::register_custom_module(Policy_head net)
{
	register_module("policy_conv", net.conv);
	register_module("policy_linear", net.lin1);
	return net;
}