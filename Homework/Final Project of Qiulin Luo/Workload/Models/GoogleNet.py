import torch
import torch.nn as nn

class Construction(nn.Module):
    def __init__(self, ord_floor, block_top, block_mid_etr, block_mid, block_btm_etr, block_btm, pool_floor):
        super(Construction, self).__init__()

        self.floor_1 = nn.Sequential(
            nn.Conv2d(ord_floor, block_top, kernel_size = 1),
            nn.BatchNorm2d(block_top),
            nn.ReLU(True),
        )
        self.floor_2 = nn.Sequential(
            nn.Conv2d(ord_floor, block_mid_etr, kernel_size = 1),
            nn.BatchNorm2d(block_mid_etr),
            nn.ReLU(True),
            nn.Conv2d(block_mid_etr, block_mid, kernel_size = 3, padding = 1),
            nn.BatchNorm2d(block_mid),
            nn.ReLU(True),
        )
        self.floor_3 = nn.Sequential(
            nn.Conv2d(ord_floor, block_btm_etr, kernel_size = 1),
            nn.BatchNorm2d(block_btm_etr),
            nn.ReLU(True),
            nn.Conv2d(block_btm_etr, block_btm, kernel_size = 3, padding = 1),
            nn.BatchNorm2d(block_btm),
            nn.ReLU(True),
            nn.Conv2d(block_btm, block_btm, kernel_size = 3, padding = 1),
            nn.BatchNorm2d(block_btm),
            nn.ReLU(True),
        )
        self.floor_4 = nn.Sequential(
            nn.MaxPool2d(3, stride = 1, padding = 1),
            nn.Conv2d(ord_floor, pool_floor, kernel_size = 1),
            nn.BatchNorm2d(pool_floor),
            nn.ReLU(True),
        )

    def forward(self, x):
        return torch.cat([self.floor_1(x), self.floor_2(x),
                          self.floor_3(x),self.floor_4(x)], 1)

class GoogLeNet(nn.Module):
    def __init__(self):
        super(GoogLeNet, self).__init__()
        self.matting_floor = nn.Sequential(
            nn.Conv2d(3, 192, kernel_size = 3, padding = 1),
            nn.BatchNorm2d(192),
            nn.ReLU(True),
        )

        self.ceil_3 = Construction(192,  64,  96, 128, 16, 32, 32)
        self.floor_3 = Construction(256, 128, 128, 192, 32, 96, 64)
        self.max_pool = nn.MaxPool2d(3, stride = 2, padding = 1)
        self.ceil_4 = Construction(480, 192,  96, 208, 16,  48,  64)
        self.floor_4 = Construction(512, 160, 112, 224, 24,  64,  64)
        self.transition_4 = Construction(512, 128, 128, 256, 24,  64,  64)
        self.transition_5 = Construction(512, 112, 144, 288, 32,  64,  64)
        self.transition_6 = Construction(528, 256, 160, 320, 32, 128, 128)
        self.transition_7 = Construction(832, 256, 160, 320, 32, 128, 128)
        self.transition_8 = Construction(832, 384, 192, 384, 48, 128, 128)
        self.avgpool = nn.AvgPool2d(8, stride = 1)
        self.linear = nn.Linear(1024, 10)

    def forward(self, x):
        x = self.matting_floor(x)
        x = self.ceil_3(x)
        x = self.floor_3(x)
        x = self.max_pool(x)
        x = self.ceil_4(x)
        x = self.floor_4(x)
        x = self.transition_4(x)
        x = self.transition_5(x)
        x = self.transition_6(x)
        x = self.max_pool(x)
        x = self.transition_7(x)
        x = self.transition_8(x)
        x = self.avgpool(x)
        x = x.view(x.size(0), -1)
        x = self.linear(x)
        return x
