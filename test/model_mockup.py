import torch, numpy as np
import torch.nn as nn
import torch.nn.functional as F

class ConvLayer(nn.Module):
    def __init__(self, inp, out, kernel):
        super(ConvLayer, self).__init__()
        self.conv1 = nn.Conv2d(inp, out, kernel, padding="same")
        self.batch = nn.BatchNorm2d(out)
        self.activ = nn.LeakyReLU()

    def forward(self, x):
        x = self.conv1(x)
        x = self.batch(x)
        x = self.activ(x)
        return x

class ResLayer(nn.Module):
    def __init__(self, inp, out, kernelsize1, kernelsize2):
        super(ResLayer, self).__init__()
        self.conv1 = nn.Conv2d(inp, out, kernelsize1, padding=(
            (kernelsize1)//2,
            (kernelsize1)//2)
        )
        self.conv2 = nn.Conv2d(out, out, kernelsize2, padding=(
            (kernelsize2)//2,
            (kernelsize2)//2)
        )
        self.batch1 = nn.LayerNorm([out, 7, 6])
        self.batch2 = nn.LayerNorm([out, 7, 6])
        self.activ = nn.LeakyReLU()

    def forward(self, x):
        x = self.conv1(x)
        x = self.batch1(x)
        y = self.conv2(x)
        y = self.batch2(y)
        return self.activ(x+y)

class ValueHead(nn.Module):
    def __init__(self, inp, hidden_size, out, kernels):
        super(ValueHead, self).__init__()
        self.conv = nn.Conv2d(inp, kernels, 1)
        self.lin1 = nn.Linear(hidden_size,out,1)
        self.lin2 = nn.Linear(out,1,1)
        self.relu = nn.LeakyReLU()
        self.tanh = nn.Tanh()
        self.size = hidden_size

    def forward(self, x):
        x = self.conv(x)
        x = torch.reshape(x, (x.shape[0], self.size))
        x = self.lin1(x)
        x = self.relu(x)
        x = self.lin2(x)
        x = self.tanh(x)
        return x

class PolicyHead(nn.Module):
    def __init__(self, inp, hidden, out):
        super(PolicyHead, self).__init__()
        self.conv = nn.Conv2d(inp, 2, 1)
        self.lin1 = nn.Linear(hidden, out, 1)
        self.activ = nn.Softmax()
        self.size = hidden

    def forward(self, x):
        x = self.conv(x)
        x = torch.reshape(x, (x.shape[0], self.size))
        x = self.lin1(x)
        return self.activ(x)

        

class Net(nn.Module):

    def __init__(self):
        super(Net, self).__init__()
        # 1 input image channel, 6 output channels, 5x5 square convolution
        # kernel
        # 
        self.conv1 = ResLayer(2,75,5,5)
        self.conv2 = ResLayer(75,75,5,5)
        self.value_head = ValueHead(75, 420, 256, 10)
        self.policy_head= PolicyHead(75, 84, 2)

    def forward(self, x):
        x = self.conv1(x)
        x = self.conv2(x)
        return self.value_head(x), self.policy_head(x)


# define ResNet with value and poliy head
net = Net()
optim = torch.optim.SGD(net.parameters(), lr=0.01)

# create input "image"
input = torch.ones(2,2,7,6)

print(input)

# run model
out = net(input.float())
print(out)

for i in range(10):
    out = net(input.float())
    loss = torch.nn.MSELoss()
    l1 = loss(out[0], torch.tensor([[1.0], [0.0]]))
    l2 = loss(out[1], torch.tensor([[0.0, 1.0], [0.0, 1.0]]))

    (l2+l1).backward()
    optim.step()


print(net(input.float()))
