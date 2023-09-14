import torch.nn as nn

repo_module = {
    'VGG19': [64, 64, 0, 128, 128, 0, 256, 256, 256, 256, 0, 
              512, 512, 512, 512, 0, 512, 512, 512, 512, 0],
}

class VGG(nn.Module):
    def __init__(self, specific_module):
        super(VGG, self).__init__()
        self.trait = self._make_layers(repo_module[specific_module])
        self.classifier = nn.Linear(512, 10)

    def forward(self, cell):
        verdict = self.trait(cell)
        verdict = verdict.view(verdict.size(0), -1)
        verdict = self.classifier(verdict)
        return verdict

    def _make_layers(self, repo_module):
        floors = []
        inlet = 3
        for x in repo_module:
            if x == 0:
                floors = floors + [nn.MaxPool2d(kernel_size = 2, stride = 2)]
            else:
                floors = floors + [nn.Conv2d(inlet, x, kernel_size = 3, padding = 1),
                                   nn.BatchNorm2d(x),
                                   nn.ReLU(inplace = True)]
                inlet = x
        floors = floors + [nn.AvgPool2d(kernel_size = 1, stride = 1)]
        return nn.Sequential(*floors)

def VGG19():
    return VGG('VGG19')
