# algo_config.json说明

    ***配置文件目前固定字段有四项，其它的配置字段根据项目拟定，下列四个字段为固定项，务必在代码接口中实现

	*字段名称：show_result  字段描述：是否实时显示图片或者视频结果 0 不显示 1 显示
	*字段名称：draw_roi_area 字段描述：是否在结果图片或者视频中画出ROI感兴趣区域 0 不画 1 画
	*字段名称：draw_result 字段描述：是否画出检测到的物体结果框 0 不画 1 画
	*字段名称：gpu_id 字段描述：把程序在第几块GPU上运行（这个参数适应GPU算法及多GPU机器，默认为0，在第一块GPU上运行，
																			为1 则在第二块GPU上运行，以此类推）
	最终的配置文件名称根据项目自拟（例如：CarDetect.json）
	{
		"show_result":0
		"draw_roi_area":0
		"draw_result":0
		"gpu_id":0
	}

# model.data
	模型数据