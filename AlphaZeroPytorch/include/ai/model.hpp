// TorchTestCMake.h : Include file for standard system include files,
// or project specific include files.

// TODO READ Alpha Go Zero paper

#pragma once

#include <iostream>
#include <game/game.hpp>
#include "memory.hpp"
#include <tuple>
#include <jce/string.hpp>
#include <string>

#define COPY(x) this-> ## x ## . ## copyModel(&model ## ->  ## x)


namespace AlphaZero {
	namespace ai {
		class TopLayer : public torch::nn::Module {
		public: torch::nn::Conv2d conv1;
		public: torch::nn::BatchNorm2d batch;
		public: torch::nn::LeakyReLU relu;
		private: int kernel1;

		public: TopLayer(int inp, int out, int kernelsize1);
		public: torch::Tensor forward(torch::Tensor);
		public: void moveTo(c10::Device* device);
		};
		class ResNet : public torch::nn::Module {
		public: torch::nn::Conv2d conv1, conv2;
		public: torch::nn::BatchNorm2d batch, batch2;
		public: torch::nn::LeakyReLU activ;
		private: int kernel1, kernel2;

		public: ResNet(int inp, int out, int kernelsize1, int kernelsize2);
		public: torch::Tensor forward(torch::Tensor);
		public: void moveTo(c10::Device* device);
		};

		class Value_head : torch::nn::Module {
		public: torch::nn::Conv2d conv;
		public: torch::nn::Linear lin1, lin2;
		public: torch::nn::ReLU relu;
		public: torch::nn::Tanh tanh;
		private: int size;

		public: Value_head(int inp, int hidden_size, int out, int kernels);
		public: torch::Tensor forward(torch::Tensor);
		public: void moveTo(c10::Device* device);
		};

		class Policy_head : torch::nn::Module {
		public: torch::nn::Conv2d conv;
		public: torch::nn::Linear lin1;
		private: int size;

		public: Policy_head(int inp, int hidden, int out);
		public: torch::Tensor forward(torch::Tensor);
		public: void moveTo(c10::Device* device);
		};

		typedef torch::nn::MSELoss Loss;
		typedef torch::optim::SGD Optimizer;
		typedef torch::optim::SGDOptions OptimizerOptions;
		
		class Model : public torch::nn::Module {
		//private: torch::nn::Conv2d headLayer;
		private: TopLayer top;
		private: ResNet res1, res2, res3, res4, res5, res6;
		private: Value_head value_head;
		private: Policy_head policy_head;

		private: bool CUDA;

		private: Loss loss;
		private: Optimizer optim;
			
		public: Model();
		public: std::pair<torch::Tensor, torch::Tensor> forward(torch::Tensor);
		public: std::pair<float, float> train(const std::pair<torch::Tensor, torch::Tensor>&x, const std::pair<torch::Tensor, torch::Tensor>&y);

		public: std::pair<float, torch::Tensor>predict(std::shared_ptr<Game::GameState> state);
		public: static std::tuple<torch::Tensor, torch::Tensor, torch::Tensor> getBatch(std::shared_ptr<Memory> memory, unsigned int batchSize);
		public: void fit(const std::tuple<torch::Tensor, torch::Tensor, torch::Tensor>& batch, const unsigned short& run, const unsigned short& trainingLoop);

		public: void save_version(unsigned int version);
		public: void save_as_current();
		public: void save_to_file(char* filename);

		public: void load_version(unsigned int version);
		public: void load_current();
		public: void load_from_file(char* filename);

		public: void copyModel(std::shared_ptr<Model>);
		public: void moveTo(c10::Device* device);

		private: TopLayer register_custom_module(TopLayer net);
		private: ResNet register_custom_module(ResNet net, std::string layer);
		private: Value_head register_custom_module(Value_head net);
		private: Policy_head register_custom_module(Policy_head net);

		};
	}
}
// customizable section
#define modelTest false

inline AlphaZero::ai::Model::Model() :
	//headLayer(register_module("conv", torch::nn::Conv2d()))
	top(this->register_custom_module(TopLayer(2, 75, 5))),
	res1(this->register_custom_module(ResNet(75, 75, 5, 5), "Residual_1")),
	res2(this->register_custom_module(ResNet(75, 75, 5, 5), "Residual_2")),
	res3(this->register_custom_module(ResNet(75, 75, 5, 5), "Residual_3")),
	res4(this->register_custom_module(ResNet(75, 75, 5, 5), "Residual_4")),
	res5(this->register_custom_module(ResNet(75, 75, 5, 5), "Residual_5")),
	res6(this->register_custom_module(ResNet(75, 75, 5, 5), "Residual_6")),
	value_head(this->register_custom_module(Value_head(75, 420, 210, 10))),
	policy_head(this->register_custom_module(Policy_head(75, 84, 42))),
	optim(Optimizer(this->parameters(), OptimizerOptions(0.0001).momentum(0.9)))
{
}

inline std::pair<torch::Tensor, torch::Tensor> AlphaZero::ai::Model::forward(torch::Tensor x)
{
	if (!x.is_cuda() && torch::cuda::cudnn_is_available()) {
		x = x.cuda();
	}
	x = this->top.forward(x);
	x = this->res1.forward(x);
	x = this->res2.forward(x);
	x = this->res3.forward(x);
	x = this->res4.forward(x);
	x = this->res5.forward(x);
	x = this->res6.forward(x);

	// compute individual heads
	torch::Tensor value = this->value_head.forward(x);
	torch::Tensor poly = this->policy_head.forward(x);
	return { value, poly };
};
// end of cutimizable section

inline AlphaZero::ai::TopLayer::TopLayer(int inp, int out, int kernelsize1) :
	conv1(torch::nn::Conv2d(torch::nn::Conv2dOptions(inp, out, kernelsize1))),
	batch(torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(out))),
	relu(torch::nn::LeakyReLU(torch::nn::LeakyReLU())),
	kernel1(kernelsize1 / 2)
{
	if (torch::cuda::cudnn_is_available())
	{
		this->moveTo(&c10::Device("cuda:0"));
	}
}
inline torch::Tensor AlphaZero::ai::TopLayer::forward(torch::Tensor x)
{
	x = torch::nn::functional::pad(x, torch::nn::functional::PadFuncOptions({ kernel1, kernel1, kernel1, kernel1 }));
	x = this->conv1(x);
	x = this->batch(x);
	x = this->relu(x);
	return x;
}

inline void AlphaZero::ai::TopLayer::moveTo(c10::Device* device)
{
	this->conv1->to(*device, true);
	this->batch->to(*device, true);
	this->relu->to(*device, true);
}

inline AlphaZero::ai::ResNet::ResNet(int inp, int out, int kernelsize1, int kernelsize2) :
	kernel1(kernelsize1), kernel2(kernelsize2),
	conv1(torch::nn::Conv2d(torch::nn::Conv2dOptions(inp, out, kernelsize1))),
	conv2(torch::nn::Conv2d(torch::nn::Conv2dOptions(out, out, kernelsize2))),
	batch(torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(out))),
	batch2(torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(out))),
	activ(torch::nn::LeakyReLU(torch::nn::LeakyReLU()))
{
	if (torch::cuda::is_available()) {
		this->moveTo(&c10::Device("cuda:0"));
	}
}

inline torch::Tensor AlphaZero::ai::ResNet::forward(torch::Tensor x)
{
#if modelTest
	std::cout << x.sizes() << std::endl;
#endif 
	x = torch::nn::functional::pad(x, torch::nn::functional::PadFuncOptions({ kernel1 / 2, kernel1 / 2, kernel1 / 2, kernel1 / 2 }));
	x = this->conv1(x);
	x = this->batch(x);
	torch::Tensor y = torch::nn::functional::pad(x, torch::nn::functional::PadFuncOptions({ kernel2 / 2, kernel2 / 2, kernel2 / 2, kernel2 / 2 }));
	y = this->conv2(y);
	y = this->batch2(y);
	return x + y;
}

inline void AlphaZero::ai::ResNet::moveTo(c10::Device* device)
{
	this->conv1->to(*device, true);
	this->conv2->to(*device, true);
	this->batch->to(*device, true);
	this->batch2->to(*device, true);
	this->activ->to(*device, true);
}

inline AlphaZero::ai::Value_head::Value_head(int inp, int hidden_size, int out, int convOut) :
	conv(torch::nn::Conv2d(torch::nn::Conv2dOptions(inp, convOut, 1))),
	lin1(torch::nn::Linear(torch::nn::LinearOptions(hidden_size, out))),
	lin2(torch::nn::Linear(torch::nn::LinearOptions(out, 1))),
	relu(torch::nn::ReLU()), tanh(torch::nn::Tanh())
{
	this->size = hidden_size;

	if (torch::cuda::is_available()) 
	{
		this->moveTo(&c10::Device("cuda:0"));
	}
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

inline void AlphaZero::ai::Value_head::moveTo(c10::Device* device)
{
	this->conv->to(*device, true);
	this->lin1->to(*device, true);
	this->relu->to(*device, true);
	this->lin2->to(*device, true);
	this->tanh->to(*device, true);
}

inline AlphaZero::ai::Policy_head::Policy_head(int inp, int hidden, int out) :
	conv(torch::nn::Conv2d(torch::nn::Conv2dOptions(inp, 2, 1))),
	lin1(torch::nn::Linear(hidden, out))
{
	this->size = hidden;

	if (torch::cuda::is_available()) {
		this->moveTo(&c10::Device("cuda:0"));
	}
}

inline torch::Tensor AlphaZero::ai::Policy_head::forward(torch::Tensor x)
{
#if modelTest
	std::cout << "poly" << std::endl;
	std::cout << x.sizes() << std::endl;
#endif 
	x = this->conv(x);
	x = this->lin1(x.reshape({ x.size(0), this->size }));
	return x;
}

inline void AlphaZero::ai::Policy_head::moveTo(c10::Device* device)
{
	this->conv->to(*device, true);
	this->lin1->to(*device, true);
}

inline std::pair<float, float> AlphaZero::ai::Model::train(const std::pair<torch::Tensor, torch::Tensor>& x, const std::pair<torch::Tensor, torch::Tensor>& y)
{
	auto valLoss = torch::mse_loss(x.first, y.first);
	auto plyLoss = torch::mse_loss(x.second, y.second); // TODO get cce  
	torch::autograd::backward({ valLoss, plyLoss });
	this->optim.step();
	return { torch::mean(valLoss).item().toFloat(), torch::mean(plyLoss).item().toFloat() };
}

inline std::pair<float, torch::Tensor> AlphaZero::ai::Model::predict(std::shared_ptr<Game::GameState> state)
{
	torch::Tensor NNInput = state->toTensor();
	if (torch::cuda::cudnn_is_available())
		NNInput = NNInput.cuda();
	std::pair<torch::Tensor, torch::Tensor> NNOut = this->forward(NNInput);
	float value = NNOut.first[0].item<float>();
	NNOut.second = NNOut.second.cpu(); // only if cuda is available ??
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
	std::pair<torch::Tensor, torch::Tensor> trueVals = this->forward(std::get<0>(batch).cuda());
	std::pair<float, float> error = this->train(trueVals, { std::get<2>(batch).cuda(), std::get<1>(batch).cuda() });
#if ModelLogger
	debug::log::modelLogger->info("model error in iteration {} on batch {} had valueError of {} and polyError of {}", run, trainingLoop, std::get<0>(error), std::get<1>(error));
#endif
}

inline void AlphaZero::ai::Model::save_version(unsigned int version)
{
	char buffer[50];
	if (this->CUDA)
		std::sprintf(buffer, "models/run_%d/V_%d.torch", runVersion, version);
	else
		std::sprintf(buffer, "models/run_%d/CPU_%d.torch", runVersion, version);
	std::cout << buffer << std::endl;
	this->save_to_file(buffer);
}

inline void AlphaZero::ai::Model::save_as_current()
{
	char buffer[50];
	if (this->CUDA)
		std::sprintf(buffer, "models/run_%d/currentModel.torch", runVersion);
	else
		std::sprintf(buffer, "models/run_%d/CPU_currentModel.torch", runVersion);
	this->save_to_file(buffer);
}

inline void AlphaZero::ai::Model::save_to_file(char* filename)
{
	torch::serialize::OutputArchive out;
	this->save(out);
	std::string model_path = std::string(filename);
	out.save_to(model_path);
}

inline void AlphaZero::ai::Model::load_version(unsigned int version)
{
	std::cout << "loading ...";
	char buffer[50];
	if (this->CUDA)
		std::sprintf(buffer, "models/run_%d/V_%d.torch", runVersion, version);
	else
		std::sprintf(buffer, "models/run_%d/CPU_%d.torch", runVersion, version);
	this->load_from_file(buffer);
	std::cout << " loaded Version " << version << std::endl;
}

inline void AlphaZero::ai::Model::load_current()
{
	char buffer[50];
	if (this->CUDA)
		std::sprintf(buffer, "models/run_%d/currentModel.torch", runVersion);
	else
		std::sprintf(buffer, "models/run_%d/CPU_currentModel.torch", runVersion);
	this->load_from_file(buffer);
}

inline void AlphaZero::ai::Model::load_from_file(char* filename)
{
	torch::serialize::InputArchive inp;
	std::string model_path = std::string(filename);
	inp.load_from(model_path);
	this->load(inp);
}

inline void AlphaZero::ai::Model::copyModel(std::shared_ptr<AlphaZero::ai::Model> model)
{
	torch::autograd::GradMode::set_enabled(false);
	auto new_params = model->named_parameters(true);
	auto params = this->named_parameters(true /*recurse*/);
	auto buffers = this->named_buffers(true /*recurse*/);
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
	torch::autograd::GradMode::set_enabled(true);
}

inline void AlphaZero::ai::Model::moveTo(c10::Device* device)
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
// TODO: Reference additional headers your program requires here.

#undef COPY
#undef COPY_CONV
#undef C