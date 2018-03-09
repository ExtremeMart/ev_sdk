
## docker 上传镜像(示例)

> 上传 vmare/nginx-photon 到 docker 仓库


![](http://img-alert.extremevision.com.cn/%E5%BE%AE%E4%BF%A1%E5%9B%BE%E7%89%87_20180208151805.png)


- docker login docker-registry.cvmart.net
> 按照提示输入用户名和密码,进行认证登录



- docker tag vmware/nginx-photon:v1.4.0-rc2 docker-registry.cvmart.net/face/nginx-photon:v1.4
> 使用 docker tag 命令为镜像打标签
> vmware/nginx-photon:v1.4.0-rc2 为本地 docker images显示的 REPOSITORY 和 TAG, 单独使用 IMAGE ID也可
> docker-registry.cvmart.net/face/nginx-photon:v1.4 由三部分组成 
 - docker-registry.cvmart.net  docker仓库域名
 - face docker仓库中项目名称,视用户权限和镜像的分类而定 //极市提供
 - nginx-photon:v1.4 镜像在docker仓库中的名称和标签, nginx-photo为名称, v1.4为标签


- docker push docker-registry.cvmart.net/face/nginx-photon:v1.4
> 上传docker镜像, 名称要与 docker tag 命令的最后一个参数保持一致


###  常见错误
 ![](http://img-alert.extremevision.com.cn/QQ%E6%88%AA%E5%9B%BE20180208153708.png)

- docker 仓库默认使用 https 连接, 但极市搭建的仓库为 http,只需在/etc/docker/daemon.json文件中加入 ```{ "insecure-registries":["docker-registry.cvmart.net"]}```， 然后重启 docker service 即可.

