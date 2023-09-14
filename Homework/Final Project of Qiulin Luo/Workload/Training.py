from os import lseek
from torchvision import transforms as transforms
from Models import *
from Calculagraph import schedule_display
import numpy as np
import torch.optim as optim
import torch.backends.cudnn as cudnn
import argparse
import torch.utils.data
import torchvision

CLASSES = ('plane', 'car', 'bird', 'cat', 'deer', 'dog', 'frog', 'horse', 'ship', 'truck')

def main():
    parser = argparse.ArgumentParser(description = "parameter reader")
    parser.add_argument('--lr', default = 0.001, type = float, help = '')
    parser.add_argument('--e', default = 20, type = int, help = '')
    parser.add_argument('--trB', default = 100, type = int, help = '')
    parser.add_argument('--teB', default = 100, type = int, help = '')
    parser.add_argument('--cuda', default = torch.cuda.is_available(), type = bool, help = '')
    filter = parser.parse_args()

    generation = Construct(filter)
    generation.run()

class Construct(object):
    def __init__(self, cell):
        self.model = None
        self.formula = None
        self.optimizer = None
        self.scheduler = None
        self.device = None
        self.lr = cell.lr
        self.epochs = cell.e
        self.cuda = cell.cuda
        self.trBatch = cell.trB
        self.teBatch = cell.teB
        self.tr_Initialization = None
        self.te_Initialization = None

    def load_data(self):
        vessel_tr = transforms.Compose([transforms.RandomHorizontalFlip(), transforms.ToTensor()])
        vessel_te = transforms.Compose([transforms.ToTensor()])
        data_tr = torchvision.datasets.CIFAR10(root='./data', train = True, download = True, transform = vessel_tr)
        self.tr_Initialization = torch.utils.data.DataLoader(dataset = data_tr, batch_size = self.trBatch, shuffle = True)
        data_te = torchvision.datasets.CIFAR10(root='./data', train = False, download = True, transform = vessel_te)
        self.te_Initialization = torch.utils.data.DataLoader(dataset = data_te, batch_size = self.teBatch, shuffle = False)

    def load_model(self):
        if self.cuda:
            self.device = torch.device('cuda')
            cudnn.benchmark = True
        else:
            self.device = torch.device('cpu')

        self.model = GoogLeNet().to(self.device)
        # Change GoogleNet() to VGG19()
        self.optimizer = optim.Adam(self.model.parameters(), lr = self.lr)
        self.scheduler = optim.lr_scheduler.MultiStepLR(self.optimizer, milestones = [75, 150], gamma = 0.5)
        self.formula = nn.CrossEntropyLoss().to(self.device)

    def train(self):
        print("Train process starts!")
        self.model.train()
        loss_sniffer = 0
        acc_sniffer = 0
        total = 0

        for batch_num, (data, target) in enumerate(self.tr_Initialization):
            data, target = data.to(self.device), target.to(self.device)
            self.optimizer.zero_grad()
            output = self.model(data)
            loss = self.formula(output, target)
            loss.backward()
            self.optimizer.step()
            loss_sniffer = loss_sniffer + loss.item()
            forecast = torch.max(output, 1)   
            total = total + target.size(0)
            acc_sniffer = acc_sniffer + np.sum(forecast[1].cpu().numpy() == target.cpu().numpy())

            schedule_display(batch_num, len(self.tr_Initialization), 'Loss: %.4f | Acc: %.3f%%'
                         % (loss_sniffer / (batch_num + 1), 100. * acc_sniffer / total))

        return loss_sniffer, acc_sniffer / total

    def test(self):
        print("Test process starts!")
        self.model.eval()
        loss_tsniffer = 0
        acc_tsniffer = 0
        total = 0

        with torch.no_grad():
            for batch_num, (data, target) in enumerate(self.te_Initialization):
                data, target = data.to(self.device), target.to(self.device)
                output = self.model(data)
                loss = self.formula(output, target)
                loss_tsniffer = loss_tsniffer + loss.item()
                forecast = torch.max(output, 1)
                total = total + target.size(0)
                acc_tsniffer = acc_tsniffer + np.sum(forecast[1].cpu().numpy() == target.cpu().numpy())

                schedule_display(batch_num, len(self.te_Initialization), 'Loss: %.4f | Acc: %.3f%% '
                             % (loss_tsniffer / (batch_num + 1), 100. * acc_tsniffer / total))

        return loss_tsniffer, acc_tsniffer / total

    def run(self):
        self.load_data()
        self.load_model()
        final_acc = 0
        for epoch in range(1, self.epochs + 1):
            self.scheduler.step(epoch)
            print("\nEpoch: %d\n" % epoch)
            train_result = self.train()
            print(train_result)
            test_result = self.test()
            final_acc = max(final_acc, test_result[1])
            if epoch == self.epochs:
                print("\nBest Accuracy is: %.3f%%" % (final_acc * 100))
                
if __name__ == '__main__':
    main()
