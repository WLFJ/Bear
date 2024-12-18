# FuzzingTorch Toolchain Extension Notes

为了适配我们的FuzzingTorch，目前对该工具提出一个新的需求：

能够根据配置文件设定给特定文件的编译命令当中注入特定的编译选项，或者作为代理执行其他命令。
