#!/usr/bin/python
# -*- coding: UTF-8 -*-
# 文件名：ji.py

#抽象类加抽象方法就等于面向对象编程中的接口
from abc import ABCMeta,abstractmethod

class ji(object):
    __metaclass__ = ABCMeta #指定这是一个抽象类
    @abstractmethod  #抽象方法
    def init(self):#返回code
        return 0
    
    def calc(self, buffer, out_file=None):#输入文件的byte内容和结果文件名，返回code和json
        return "json", 0
