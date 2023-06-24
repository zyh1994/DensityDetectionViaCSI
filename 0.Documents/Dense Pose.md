# Authors

* Jiaqi Geng jiaqigen@andrew.cmu.edu Carnegie Mellon University Pittsburgh, PA, USA
* Dong Huang donghuang@cmu.edu Carnegie Mellon University Pittsburgh, PA, USA
* Fernando De la Torre ftorre@cs.cmu.edu Carnegie Mellon University Pittsburgh, PA, USA

# Bibtex
Geng J, Huang D, De la Torre F. DensePose From WiFi[J]. arXiv preprint arXiv:2301.00250, 2022.

# 0. ABSTRACT

Advances in computer vision and machine learning techniques have led to significant development in 2D and 3D human pose estimation from RGB cameras, LiDAR, and radars. However, human pose esti- mation from images is adversely affected by occlusion and lighting, which are common in many scenarios of interest. Radar and LiDAR technologies, on the other hand, need specialized hardware that is expensive and power-intensive. Furthermore, placing these sensors in non-public areas raises significant privacy concerns.

> 计算机视觉和机器学习技术的进步已经导致RGB摄像头、LiDAR和雷达中2D和3D人体姿势估计的重大发展。然而，图像中的人体姿势估计受到遮挡和照明的不利影响，这在许多关注的场景中是常见的。另一方面，雷达和LiDAR技术需要专门的硬件，价格昂贵，耗能大。此外，在非公共区域放置这些传感器会引发严重的隐私问题。

To address these limitations, recent research has explored the use of WiFi antennas (1D sensors) for body segmentation and key-point body detection. This paper further expands on the use of the WiFi signal in combination with deep learning architectures, commonly used in computer vision, to estimate dense human pose correspon- dence. We developed a deep neural network that maps the phase and amplitude of WiFi signals to UV coordinates within 24 human regions. The results of the study reveal that our model can estimate the dense pose of multiple subjects, with comparable performance to image-based approaches, by utilizing WiFi signals as the only input. This paves the way for low-cost, broadly accessible, and privacy-preserving algorithms for human sensing.

> 为了解决这些局限，最近的研究已经探索了使用WiFi天线（1D传感器）进行身体分割和关键点身体检测。本文进一步拓展了WiFi信号与计算机视觉中常用的深度学习体系结构的结合，以估计密集的人体姿势对应关系。我们开发了一种深度神经网络，将WiFi信号的相位和振幅映射到24个人体区域内的UV坐标。研究结果表明，我们的模型可以利用WiFi信号作为唯一输入，估计多个对象的密集姿势，与基于图像的方法具有相当的性能。这为低成本、广泛可访问和隐私保护的人体传感算法铺平了道路。

**Keywords**

Pose Estimation, Dense Body Pose Estimation, WiFi Signals, Key- point Estimation, Body Segmentation, Object Detection, UV Coor- dinates, Phase and Amplitude, Phase Sanitization, Channel State Information, Domain Translation, Deep Neural Network, Mask- RCNN


# 1. INTRODUCTION

Much progress has been made in human pose estimation using 2D [7, 8, 12, 22, 28, 33] and 3D [17, 32] sensors in the last few years (e.g., RGB sensors, LiDARs, radars), fueled by applications in autonomous driving and augmented reality. These traditional sensors, however, are constrained by both technical and practical considerations. LiDAR and radar sensors are frequently seen as being out of reach for the average household or small business due to their high cost. For example, the medium price of one of the most common COTS LiDAR, Intel L515, is around 700 dollars, and the prices for ordinary radar detectors range from 200 dollars to 600 dollars. In addition, these sensors are too power-consuming for daily and household use. As for RGB cameras, narrow field of view and poor lighting conditions, such as glare and darkness, can have a severe impact on camera-based approaches. Occlusion is another obstacle that prevents the camera-based model from generating reasonable pose predictions in images. This is especially worrisome for indoors scenarios, where furniture typically occludes people.

> 近年来，人体姿势估计在2D [7, 8, 12, 22, 28, 33] 和3D [17, 32]传感器方面取得了很大进展（例如，RGB传感器，LiDAR，雷达），得益于自动驾驶和增强现实应用。然而，这些传统传感器受到技术和实际限制的限制。LiDAR和雷达传感器因其高成本常常被认为不适合普通家庭或小企业使用。例如，最常见的COTS LiDAR之一Intel L515的中等价格约为700美元，而普通雷达探测器的价格从200美元到600美元不等。此外，这些传感器对于日常和家庭使用来说太耗电。至于RGB相机，狭窄的视野和较差的照明条件，例如眩光和黑暗，可能对基于相机的方法产生严重影响。遮挡是另一个阻碍相机模型在图像中生成合理姿势预测的障碍。这对室内场景尤其令人担忧，因为家具通常会遮挡人。

More importantly, privacy concerns prevent the use of these technologies in non-public places. For instance, most people are uncomfortable with having cameras recording them in their homes, and in certain areas (such as the bathroom) it will not be feasible to install them. These issues are particularly critical in healthcare applications, that are increasingly shifting from clinics to homes, where people are being monitored with the help of cameras and other sensors. It is important to resolve the aforementioned prob- lems in order to better assist the aging population, which is the most susceptible (especially during COVID) and has a growing demand to keep them living independently at home.

> 此外，隐私问题阻止了这些技术在非公共地方的使用。例如，大多数人不愿意在家中被摄像机录制，在某些地区（如浴室）安装摄像机将不可行。这些问题在医疗应用中特别关键，随着医疗监测从诊所向家庭转移，人们正在通过摄像机和其他传感器监测。为了更好地帮助老年人，特别是在COVID期间，最容易受到影响并需要独自生活在家中的老年人，解决上述问题至关重要。

We believe that WiFi signals can serve as a ubiquitous substitute for RGB images for human sensing in certain instances. Illumination and occlusion have little effect on WiFi-based solutions used for interior monitoring. In addition, they protect individuals’ privacy and the required equipment can be bought at a reasonable price. In fact, most households in developed countries already have WiFi at home, and this technology may be scaled to monitor the well-being of elder people or just identify suspicious behaviors at home.

> 我们相信，WiFi信号可以在某些情况下作为RGB图像对人体感知的普遍替代品。光照和遮挡对用于室内监测的基于WiFi的解决方案几乎没有影响。此外，它们保护个人的隐私，所需的设备价格合理。事实上，大多数发达国家的家庭已经有了WiFi，这项技术可以扩展到监测老年人的健康状况，或在家中识别可疑行为。

The issue we are trying to solve is depicted in Fig. 1 (first row). Given three WiFi transmitters and three aligned receivers, can we detect and recover dense human pose correspondence in clut- tered scenarios with multiple people (Fig. 1 fourth row). It should be noted that many WiFi routers, such as TP-Link AC1750, come with 3 antennas, so our method only requires 2 of these routers. Each of these router is around 30 dollars, which means our entire setup is still way cheaper than LiDAR and radar systems. Many factors make this a difficult task to solve. 

> 我们试图解决的问题如图1（第1行）所示。给定三个WiFi发射器和三个对齐的接收器，我们是否可以在多人的杂乱情况下（图1第4行）检测和恢复人体密集姿态对应关系。应该注意到，许多WiFi路由器（例如TP-Link AC1750）配备了3个天线，因此我们的方法只需要这些路由器中的2个。每个路由器的价格约为30美元，这意味着我们整个设置仍然远远低于LiDAR和雷达系统。许多因素使这成为一个难以解决的任务。

First of all, WiFi-based perception[11, 30] is based on the Channel-state-information (CSI) that represents the ratio between the transmitted signal wave and the received signal wave. The CSIs are complex decimal sequences that do not have spatial correspondence to spatial locations, such as the image pixels. 

> 首先，基于WiFi的感知[11,30]是基于信道状态信息（CSI）的，该信息表示发送信号波和接收信号波的比率。 CSI是复数十进制序列，没有与空间位置的对应关系，例如图像像素。

Secondly, classic techniques rely on accurate measurement of time-of-fly and angle-of-arrival of the signal be- tween the transmitter and receiver [13, 26]. These techniques only locate the object’s center; moreover, the localization accuracy is only around 0.5 meters due to the random phase shift allowed by the IEEE 802.11n/ac WiFi communication standard and potential interference with electronic devices under similar frequency range such as microwave oven and cellphones.

> 其次，经典技术依赖于信号在发射机和接收机之间的时间和角度测量[13,26]。 这些技术只能定位物体的中心；此外，由于IEEE 802.11n/ac WiFi通信标准允许的随机相位变化以及与具有相似频率范围的电子设备（例如微波炉和手机）的干扰，定位精度仅为0.5米左右。

To address these issues, we derive inspiration from recent pro-posed deep learning architectures in computer vision, and propose a neural network architecture that can perform dense pose estima- tion from WiFi. Fig 1 (bottom row) illustrates how our algorithm is able to estimate dense pose using only WiFi signal in scenarios with occlusion and multiple people.

> 为了解决这些问题，我们从计算机视觉中最近提出的深度学习架构中获得启示，并提出了一种能够从WiFi进行密集姿态估计的神经网络架构。 图1（底部）说明了我们的算法如何仅使用WiFi信号在存在遮挡和多人的情况下估计密集姿态。

![在这里插入图片描述](https://img-blog.csdnimg.cn/5779ba067584485c9a60db72410e8a3a.png#pic_center)
 *Figure 1: The first row illustrates the hardware setup. The second and third rows are the clips of amplitude and phase of the input WiFi signal. The fourth row contains the dense pose estimation of our algorithm from only the WiFi signal.*

> 图1：第一行说明了硬件设置。第二行和第三行是输入WiFi信号的振幅和相位的剪辑。第四行包含了我们算法仅通过WiFi信号的密集姿态估计。

# 2 RELATED WORK

This section briefly describes existing work on dense estimation from images and human sensing from WiFi.

> 这一部分简要描述了从图像中进行密集估计和从WiFi中进行人类感知的现有工作。

Our research aims to conduct dense pose estimation via WiFi. In computer vision, the subject of dense pose estimation from pictures and video has received a lot of attention [6, 8, 18, 40]. This task consists of finding the dense correspondence between image pixels and the dense vertices indexes of a 3D human body model. The pioneering work of Güler et al. [8] mapped human images to dense correspondences of a human mesh model using deep networks. DensePose is based on instance segmentation architectures such as Mark-RCNN [9], and predicts body-wise UV maps for each pixel, where UV maps are flattened representations of 3d geometry, with coordinate points usually corresponding to the vertices of a 3d dimensional object. In this work, we borrow the same architecture as DensePose [8]; however, our input will not be an image or video, but we use 1D WiFi signals to recover the dense correspondence.

> 我们的研究旨在通过WiFi进行密集姿态估计。在计算机视觉中，从图片和视频中进行密集姿态估计的问题受到了很多关注 [6,8,18,40]。该任务包括找到图像像素和三维人体模型的密集顶点索引之间的密集对应关系。 Güler等人的先驱工作[8]使用深度网络将人类图像映射到人体网格模型的密集对应关系。DensePose基于实例分割架构，如Mark-RCNN [9]，预测每个像素的body-wise UV映射，其中UV映射是3d几何的压缩表示，其坐标点通常对应3d维对象的顶点。在这项工作中，我们借鉴了与DensePose [8]相同的架构；然而，我们的输入不是图像或视频，而是使用1D WiFi信号来恢复密集对应关系。

Recently, there have been many extensions of DensePose pro- posed, especially in 3D human reconstruction with dense body parts [3, 35, 37, 38]. Shapovalov et al.’s [24] work focused on lifting dense pose surface maps to 3D human models without 3D supervision. Their network demonstrates that the dense correspondence alone (without using full 2D RGB images) contains sufficient information to generate posed 3D human body. Compared to previous works on reconstructing 3D humans with sparse 2D keypoints, DensePose annotations are much denser and provide information about the 3D surface instead of 2D body joints.

> 最近，DensePose 的许多扩展已经被提出，特别是在密集身体部分的 3D 人体重建方面 [3,35,37,38]。Kapovalov 等人的工作 [24] 着重于将密集姿态表面映射升级为不带 3D 监督的 3D 人体模型。他们的网络证明，仅密集对应（不使用完整的 2D RGB 图像）包含生成 posed 3D 人体所需的充分信息。与之前用稀疏 2D 关键点重建 3D 人类的工作相比，DensePose 注释更密集，提供了有关 3D 表面的信息，而不是 2D 身体关节。

While there is a extensive literature on detection [19, 20], tracking [4, 34], and dense pose estimation [8, 18] from images and videos, human pose estimation from WiFi or radar is a relatively unexplored problem. At this point, it is important to differentiate the current work on radar-based systems and WiFi. The work of Adib et.al. [2] proposed a Frequency Modulated Continuous Wave (FMCW) radar system (broad bandwidth from 5.56GHz to 7.25GHz) for indoor human localization. A limitation of this system is the specialized hardware for synchronizing the ransmission, refraction, and reflection to compute the Time-of-Flight (ToF). The system reached a resolution of 8.8 cm on body localization. In the following work [1], they improved the system by focusing on a moving per- son and generated a rough single-person outline with depth maps. Recently, they applied deep learning approaches to do fine-grained human pose estimation using a similar system, named RF-Pose [39]. These systems do not work under the IEEE 802.11n/ac WiFi communication standard (40MHz bandwidth centered at 2.4GHz). They rely on additional high-frequency and high-bandwidth electromagnetic fields, which need specialized technology not available to the general public. Recently, significant improvements have been made to radar-based human sensing systems. mmMesh [36] generates 3D human mesh from commercially portable millimeter-wave de- vices. This system can accurately localize the vertices on the human mesh with an average error of 2.47 cm. However, mmMesh does not work well with occlusions since high-frequency radio waves cannot penetrate objects.

> 在图像和视频的检测[19,20]、跟踪[4,34]和密集姿态估计[8,18]方面有着丰富的文献，但从WiFi或雷达中估计人体姿态是一个相对未被探索的问题。在这一点上，重要的是要区分当前基于雷达的系统和WiFi的工作。 Adib等人的工作[2]提出了一种频率调制连续波（FMCW）雷达系统（从5.56GHz到7.25GHz的宽带）用于室内人体定位。该系统的一个限制是用于同步传输、折射和反射以计算时间到达（ToF）的专用硬件。该系统在身体定位上达到了8.8厘米的分辨率。在随后的工作[1]中，他们通过关注移动的人来改进该系统，并生成了一个带深度图的粗略的单人轮廓。最近，他们应用深度学习方法，使用类似的系统进行细粒度人体姿态估计，称为RF-Pose[39]。这些系统不适用于IEEE 802.11n/ac WiFi通信标准（以2.4GHz为中心的40MHz带宽）。它们依赖于额外的高频和高带宽电磁场，需要普通公众不具备的专用技术。最近，基于雷达的人体传感系统取得了重大进展。

Unlike the above radar systems, the WiFi-based solution [11, 30] used off-the-shelf WiFi adapters and 3dB omnidirectional antennas. The signal propagate as the IEEE 802.11n/ac WiFi data packages transmitting between antennas, which does not introduce addi- tional interference. However, WiFi-based person localization using the traditional time-of-flight (ToF) method is limited by its wave- length and signal-to-noise ratio. Most existing approaches only conduct center mass localization [5, 27] and single-person action classification [25, 29]. Recently, Fei Wang et.al. [31] demonstrated that it is possible to detect 17 2D body joints and perform 2D se- mantic body segmentation mask using only WiFi signals. In this work, we go beyond [31] by estimating dense body pose, with much more accuracy than the 0.5m that the WiFi signal can pro- vide theoretically. Our dense posture outputs push above WiFi’s signal constraint in body localization, paving the road for complete dense 2D and possibly 3D human body perception through WiFi. To achieve this, instead of directly training a randomly initialized WiFi-based model, we explored rich supervision information to improve both the performance and training efficiency, such as uti- lizing the CSI phase, adding keypoint detection branch, and transfer learning from the image-based model.

> 与上述的雷达系统不同，基于WiFi的解决方案[11,30]使用了现成的WiFi适配器和3dB全向天线。信号传播是IEEE 802.11n/ac WiFi数据包在天线之间传输，不会引入额外的干扰。然而，使用传统的时间-飞行（ToF）方法的WiFi基于人员定位受其波长和信噪比的限制。大多数现有的方法仅进行中心质量定位[5,27]和单人行动分类[25,29]。最近，Fei Wang等人[31]证明了可以使用仅WiFi信号检测17个2D身体关节并执行2D语义身体分割掩码。在这项工作中，我们超越了[31]，通过估计密集的身体姿态，比WiFi信号可以提供的理论上的0.5米更精确。我们密集的姿势输出超出了WiFi在身体定位中的信号限制，为完整的密集2D和可能的3D人体感知通过WiFi铺平了道路。为了实现这一目标，我们不是直接训练随机初始化的基于WiFi的模型，而是探索丰富的监督信息来提高性能和训练效率，例如利用CSI相位，添加关键点检测分支，并从基于图像的模型进行转移学习。

# 3 METHODS

Our approach produces UV coordinates of the human body surface from WiFi signals using three components: first, the raw CSI signals are cleaned by amplitude and phase sanitization. Then, a two-branch encoder-decoder network performs domain translation from sanitized CSI samples to 2D feature maps that resemble images. The 2D features are then fed to a modified DensePose-RCNN architecture [8] to estimate the UV map, a representation of the dense correspondence between 2D and 3D humans. To improve the training of our WiFi-input network, we conduct transfer learning, where we minimize the differences between the multi-level fea- ture maps produced by images and those produced by WiFi signals before training our main network.

> 我们的方法通过三个部分从WiFi信号生成人体的UV坐标：首先，原始CSI信号通过振幅和相位清洁处理；然后，一个双分支编码器-解码器网络从清洁的CSI样本进行域名转换，得到类似图像的2D特征图；最后，2D特征图输入到修改过的DensePose-RCNN体系结构[8]中，估计UV图，表示2D和3D人体之间密集对应关系的表示。为了提高我们WiFi输入网络的训练，我们进行迁移学习，通过最小化图像和WiFi信号生成的多层特征图的差异，在训练主网络之前提高特征图的相似度。

The raw CSI data are sampled in 100Hz as complex values over 30 subcarrier frequencies (linearly spaced within $2.4GHz \pm 20MHz$) transmitting among 3 emitter antennas and 3 reception antennas (see Figure 2). Each CSI sample contains a $3 \times 3$ real integer matrix and a $3 \times 3$ imaginary integer matrix. The inputs of our network contained 5 consecutive CSI samples under 30 frequencies, which are organized in a $150 \times 3 \times 3$ amplitude tensor and a $150 \times 3 \times 3$ phase tensor respectively. Our network outputs include a $17 \times 56 \times 56$ tensor of keypoint heatmaps (one $56 \times 56$ map for each of the 17 kepoints) and a $25 \times 112 \times 112$ tensor of UV maps (one $112 \times 112$ map for each of the 24 body parts with one additional map for background).

> 我们的原始 CSI 数据是在 100Hz 的频率以复数值形式对 30 个子载波频率（在 $2.4GHz \pm 20MHz$ 范围内线性分布）采样的。其中，3 个发射天线和 3 个接收天线之间的数据传输（见图 2）。每个 CSI 样本包含一个 $3 \times 3$ 的实整数矩阵和一个 $3 \times 3$ 的虚整数矩阵。我们的网络的输入包含 5 个连续的 CSI 样本以及 30 个频率，它们以 $150 \times 3 \times 3$ 的幅值张量和 $150 \times 3 \times 3$ 的相位张量的形式组织。我们的网络输出包括一个 $17 \times 56 \times 56$ 的关键点热图张量（每个 17 个关键点有一个 $56 \times 56$ 的地图）和一个 $25 \times 112 \times 112$ 的 UV 地图张量（每个 24 个人体部位有一个 $112 \times 112$ 的地图，另外还有一个背景图）。

## 3.1 Phase Sanitization

The raw CSI samples are noisy with random phase drift and flip (see Figure 3(b)). Most WiFi-based solutions disregard the phase of CSI signals and rely only on their amplitude (see Figure 3 (a)). As shown in our experimental validation, discarding the phase information have a negative impact on the performance of our model. In this section, we perform sanitization to obtain stable phase values to enable full use of the CSI information.

> 原始的 CSI 样本具有随机相位漂移和翻转的噪音（见图 3(b)）。大多数基于 WiFi 的解决方案忽略 CSI 信号的相位，仅依赖它们的振幅（见图 3(a)）。如我们的实验验证所示，丢弃相位信息会对我们模型的性能产生负面影响。在本节中，我们执行清洁以获得稳定的相位值，以实现对 CSI 信息的充分利用。

![在这里插入图片描述](https://img-blog.csdnimg.cn/be2eaefd972c4568a154f7923a42ea5e.png#pic_center)

*Figure 2: CSI samples from Wifi. (a) the layout of WiFi devices and human bodies, and (b) the $3 \times 3$ tensor dimension corresponds to the $3 \times 3$ transmitter-receiver antenna pairs. For instance, $E1$ denotes the first emitter and $R1$ denotes the first receiver, etc. By incorporating the 5 consecutive complex-valued CSI samples (100 samples/second) under 30 subcarrier frequencies, the two input tensors to our network are a $150 \times 3 \times 3$ amplitude tensor and a $150 \times 3 \times 3$ phase tensor.*

> 图2：WiFi的CSI样本。（a）WiFi设备和人体的布局，（b）$3 \times 3$ 张量维度对应于$3 \times 3$ 发射机-接收机天线对。例如，$E1$表示第一个发射机，$R1$表示第一个接收机，等。通过结合30个子载波频率下的5个连续复杂值CSI样本（每秒100个样本），我们网络的两个输入张量是 $150 \times 3 \times 3$ 振幅张量和 $150 \times 3 \times 3$ 相位张量。

In raw CSI samples (5 consecutive samples visualized in Figure 3(a-b)), the amplitude (A) and phase ($\Phi$) of each complex element $z = a +bi$ are computed using the formulation $𝐴 = \sqrt{a^2 + b^2}$ and $\Phi = \arctan(b/a)$. Note that the range of the arctan function is from $−\pi$ to $\pi$ and the phase values outside this range get wrapped, leading to a discontinuity in phase values. Our first sanitization step is to unwrap the phase following [10]:

> 在原始的CSI样本（在图3（a-b）中可视化的5个连续样本）中，每个复数元素$z=a+bi$的振幅（A）和相位（$\Phi$）计算使用以下公式：$A=\sqrt{a^2 + b^2}$ 和 $\Phi=\arctan(b/a)$。请注意，arctan函数的范围从$-\pi$到$\pi$，超出此范围的相位值被包装，导致相位值不连续。我们的第一步清理步骤是根据[10]拆开相位：

**Eq. 1**
$$
\begin{matrix}
\Delta \phi_{i,j} = \Phi_{i, j+1} - \Phi_{i,j} \\
if \Delta \phi_{i, j} > \pi, \Phi_{i, j+1} = \Phi_{i, j} + \Delta \phi_{i, j} - 2 \pi \\
if \Delta \phi_{i, j} < -\pi, \Phi_{i, j+1} = \Phi_{i, j} + \Delta \phi_{i, j} + 2 \pi
\end{matrix}
$$

where $i$ denotes the index of the measurements in the five consecutive samples, and $j$ denotes the index of the subcarriers(frequencies). Following unwrapping, each of the flipping phase curves in Fig- ure 3(b) are restored to continuous curves in Figure 3(c).

> 其中，$i$ 表示测量在连续五个样本中的索引，$j$ 表示子载波（频率）的索引。在解除翻转相位曲线之后，图 3(b) 中的曲线将在图 3(c) 中被恢复为连续曲线。

Observe that among the 5 phase curves captured in 5 consecutive samples in Figure 3(c), there are random jiterings that break the temporal order among the samples. To keep the temporal order of signals, previous work [23] mentioned linear fitting as a popular approach. However, directly applying linear fitting to Figure 3(c) further amplified the jitering instead of fixing it (see the failed results in Figure 3(d)).

> 在图3(c)中，可以看到5个样本中的5个相位曲线，存在随机抖动，打破了样本之间的时间顺序。根据以前的研究[23]，保持信号的时间顺序是通过线性拟合的一种常用方法。但是，直接对图3(c)进行线性拟合并不能解决问题，反而会加剧抖动（请参见图3(d)的失败结果）。

From Figure 3(c), we use median and uniform filters to eliminate outliers in both the time and frequency domain which leads to Figure 3(e). Finally, we obtain the fully sanitized phase values by applying the linear fitting method following the equations below:

> 我们在图3(c)中使用中位数和均匀滤波器，在时间和频率域中消除异常值，得到图3(e)。最后，我们通过应用以下方程来获得完全经过消毒的相位值：

**Eq. 2**

$$
\begin{matrix}
a_1 = \frac{\Phi_{F} - \Phi_{1}}{2 \pi F} \\
a_0 = \frac{1}{F} \sum_{1 \leq f \leq F} \phi_f \\
\hat{\phi_f} = \phi_f - (a_1 f + a_0)
\end{matrix}
$$

where $F$ denotes the largest subcarrier index (30 in our case) and $\hat{\phi_f}$ is the sanitized phase values at subcarrier $f$ (the f th frequency).

> 这里，$F$表示子载波索引的最大值（在我们的情况下为30），$\hat{\phi_f}$是子载波$f$（第$f$个频率）上的经过纯化的相位值。

![在这里插入图片描述](https://img-blog.csdnimg.cn/10a8fdea5a8d4759963e430912deb627.png#pic_center)
*Figure 3: Sanitization steps of CSI sequences described in Section 3.1. In each subfigure, we plot five consecutive samples (five colored curves) each containing CSI data of 30 IEEE 802.11n/ac sub-Carrier frequencies (horizontal axis).*

> 图3：在3.1节中描述的CSI序列的消毒步骤。在每个子图中，我们绘制了五个连续的样本（五条彩色曲线），每个样本包含30个IEEE 802.11n/ac子载波频率的CSI数据（水平轴）。

![在这里插入图片描述](https://img-blog.csdnimg.cn/a23101b608fa4019813d93e6ab1f79d9.png#pic_center)
*Figure 4: Modality Translation Network. Two encoders extract the features from the amplitude and phase in the CSI domain. Then the features are fused and reshaped before going through an encoder-decoder network. The output is a 3 × 720 × 1280 feature map in the image domain.*

> 图4：模态转换网络。两个编码器从CSI域的振幅和相位中提取特征。然后融合和重塑这些特征，再通过编码器-解码器网络处理。输出是图像域中3×720×1280的特征图。

## 3.2 Modality Translation Network

In order to estimate the UV maps in the spatial domain from the 1D CSI signals, we first transform the network inputs from the CSI domain to the spatial domain. This is done with the Modality Translation Network (see Figure 4). We first extract the CSI latent space features using two encoders, one for the amplitude tensor and the other for the phase tensor, where both tensors have the size of $150 \times 3 \times 3$ (5 consecutive samples, 30 frequencies, 3 emitters and 3 receivers). Previous work on human sensing with WiFi [30] stated that Convolutional Neural Network (CNN) can be used to extract spatial features from the last two dimensions (the $3 \times 3$ transmitting sensor pairs) of the input tensors. 

> 为了从1D CSI信号中估计出空间域中的UV映射，我们首先将网络输入从CSI域转换到空间域。这是通过模态转换网络（见图4）完成的。我们首先使用两个编码器提取CSI潜在空间特征，一个用于振幅张量，另一个用于相位张量，其中两个张量的大小为150 × 3 × 3（5个连续样本，30个频率，3个发射器和3个接收器）。关于WiFi的人类感知的先前研究[30]指出，可以使用卷积神经网络（CNN）从输入张量的最后两个维度（3 × 3传输传感器对）中提取空间特征。

We, on the other hand, believe that locations in the $3 \times 3$ feature map do not correlate with the locations in the 2D scene. More specifically, as depicted in Figure 2(b), the element that is colored in blue represents a 1D summary of the entire scene captured by emitter 1 and receiver 3 (E1 - R3), instead of local spatial information of the top right corner of the 2D scene. Therefore, we consider that each of the 1350 elements (in both tensors) captures a unique 1D summary of the entire scene. Following this idea, the amplitude and phase tensors are flattened and feed into two separate multi-layer perceptrons (MLP) to obtain their features in the CSI latent space. We concatenated the 1D features from both encoding branches, then the combined tensor is fed to another MLP to perform feature fusion.

> 我们相信，$3 \times 3$ 特征映射中的位置与二维场景中的位置不相关。更具体地，如图 2(b) 所示，蓝色元素代表由发射器 1 和接收器 3 (E1 - R3) 捕捉的整个场景的 1D 摘要，而不是二维场景右上角的局部空间信息。因此，我们认为两个张量中的每个 1350 个元素都捕捉了整个场景的独特 1D 摘要。遵循这个思想，振幅和相位张量被展平，并输入到两个独立的多层感知器（MLP）中，以获得其在 CSI 潜在空间中的特征。我们将两个编码分支的 1D 特征连接起来，再将组合的张量输入到另一个 MLP 中，以执行特征融合。

The next step is to transform the CSI latent space features to feature maps in the spatial domain. As shown in Figure 4, the fused 1D feature is reshaped into a 24 × 24 2D feature map. Then, we extract the spatial information by applying two convolution blocks and obtain a more condensed map with the spatial dimension of 6×6. Finally, four deconvolution layers are used to upsample the encoded feature map in low dimensions to the size of 3 × 720 × 1280. We set such an output tensor size to match the dimension commonly used in RGB-image-input network. We now have a scene representation in the image domain generated by WiFi signals.

> 下一步是将 CSI 潜在空间特征转换为空间域内的特征图。如图4所示，融合的1D特征被重塑成一个24×24的2D特征图。然后，我们通过应用两个卷积块来提取空间信息，并获得具有空间维度为6×6的更紧凑的图。最后，四个反卷积层被用来将编码的低维特征图上采样到3×720×1280的大小。我们将这样的输出张量大小设置为与常用的RGB图像输入网络的维度相匹配。我们现在已经有了一个通过WiFi信号生成的图像域内的场景表示。

## 3.3 WiFi-DensePose RCNN

After we obtain the $3 \times 720 \times 1280$ scene representation in the image domain, we can utilize image-based methods to predict the UV maps of human bodies. State-of-the-art pose estimation algorithms are two-stage; first, they run an independent person detector to estimate the bounding box and then conduct pose estimation from person-wise image patches. However, as stated before, each element in our CSI input tensors is a summary of the entire scene. It is not possible to extract the signals corresponding to a single person from a group of people in the scene. Therefore, we decide to adopt a network structure similar to DensePose-RCNN [8], since it can predict the dense correspondence of multiple humans in an end-to-end fashion.

> 在我们获得图像域中大小为 $3 \times 720 \times 1280$ 的场景表示后，我们可以使用基于图像的方法预测人体的 UV 图。最先进的姿态估计算法是分两步进行的：首先，它们运行独立的人物检测器来估计边界框，然后对以人为基准的图像块进行姿态估计。然而，正如前面所说，我们 CSI 输入张量中的每个元素都是整个场景的概括。无法从场景中的一群人中提取单个人的信号。因此，我们决定采用类似于 DensePose-RCNN [8] 的网络结构，因为它可以以端到端的方式预测多个人的密集对应关系。

More specifically, in the WiFi-DensePose RCNN (Figure 5), we extract the spatial features from the obtained $3 \times 720 \times 1280$ image-like feature map using the ResNet-FPN backbone [14]. Then, the output will go through the region proposal network [20]. To bet- ter exploit the complementary information of different sources, the next part of our network contains two branches: DensePose head and Keypoint head. Estimating keypoint locations is more reliable than estimating dense correspondences, so we can train our network to use keypoints to restrict DensePose predictions from getting too far from the body joints of humans. 

> 具体来说，在WiFi-DensePose RCNN（图5）中，我们使用ResNet-FPN骨干[14]从获得的$3 \times 720 \times 1280$图像类特征图中提取空间特征。然后，输出将通过区域提议网络[20]。为了更好地利用不同来源的补充信息，我们的网络的下一部分包含两个分支：DensePose头和关键点头。估计关键点位置比估计密集对应更可靠，因此我们可以训练我们的网络，使其使用关键点限制DensePose预测，使其不会远离人类身体关节。

The DensePose head utilizes a Fully Convolutional Network (FCN) [16] to densely predict human part labels and surface coordinates (UV coordinates) within each part, while the keypoint head uses FCN to estimate the keypoint heatmap. The results are combined and then fed into the refinement unit of each branch, where each refinement unit consists of two convolutional blocks followed by an FCN. The network outputs a $17 \times 56 \times 56$ keypoint mask and a $25 \times 112 \times 112$ IUV map. The process is demonstrated in Figure 5. It should be noted that the modality translation network and the WiFi-DensePose RCNN are trained together.

> DensePose head 使用完全卷积网络（FCN） [16] 密集地预测每个部分内的人体部分标签和表面坐标（UV 坐标），而 Keypoint head 使用 FCN 估计关键点热图。结果结合在一起，然后输入到每个分支的细化单元中，其中每个细化单元由两个卷积块和一个 FCN 组成。网络输出 $17 \times 56 \times 56$ 关键点掩码和 $25 \times 112 \times 112$ IUV 图。这个过程在图 5 中被证明。应该注意的是，模态转换网络和 WiFi-DensePose RCNN 一起被训练。

## 3.4 Transfer Learning
Training the Modality Translation Network and WiFi-DensePose RCNN network from a random initialization takes a lot of time (roughly 80 hours). To improve the training efficiency, we conduct transfer learning from an image-based DensPose network to our WiFi-based network (See Figure 6 for details).

> 从随机初始化训练Modality Translation Network和WiFi-DensePose RCNN网络需要大量时间（大约80小时）。为了提高训练效率，我们从基于图像的DensPose网络转移学习到我们的基于WiFi的网络（详情请参见图6）。

The idea is to supervise the training of the WiFi-based network with the pre-trained image-based network. Directly initializing the WiFi-based network with image-based network weights does not work because the two networks get inputs from different domains (image and channel state information). Instead, we first train an image-based DensePose-RCNN model as a teacher network. Our student network consists of the modality translation network and the WiFi-DensePose RCNN. We fix the teacher network weights and train the student network by feeding them with the synchronized images and CSI tensors, respectively. We update the student network such that its backbone (ResNet) features mimic that of our teacher network. Our transfer learning goal is to minimize the differences of multiple levels of feature maps generated by the student model and those generated by the teacher model. Therefore we calculate the mean squared error between feature maps. The transfer learning loss from the teacher network to the student network is:

> 该想法是监督WiFi基础网络的训练与预先训练的基于图像的网络。直接用基于图像的网络权重初始化基于WiFi的网络不起作用，因为这两个网络的输入来自不同的域（图像和信道状态信息）。相反，我们首先将基于图像的DensePose-RCNN模型训练为教师网络。我们的学生网络包括变态转换网络和WiFi-DensePose RCNN。我们固定教师网络的权重，通过向学生网络分别提供同步的图像和CSI张量进行训练。我们更新学生网络，使其骨架（ResNet）特征模仿教师网络。我们的转移学习目标是最小化学生模型生成的多个特征图与教师模型生成的特征图之间的差异。因此，我们计算特征图之间的均方误差。从教师网络到学生网络的转移学习损失为：

**Eq. 3**

$$
L_{tr} = MSE(P_2, P_2^*) + MSE(P_3, P_3^*) + MSE(P_4, P_4^*) + MSE(P_5, P_5^*)
$$

where $𝑀𝑆𝐸(\cdot)$ computes the mean squared error between two fea- ture maps, $\{ P_2, P_3, P_4, P_5 \}$ is a set of feature maps produced by the teacher network [14], and $\{ P_2^*, P_3^*, P_4^*, P_5^* \}$ is the set of feature maps produced by the student network [14].

> 其中，$MSSE(\cdot)$ 计算两个特征图之间的均方误差，${P_2, P_3, P_4, P_5}$ 是由教师网络[14]生成的一组特征图，${P_2^, P_3^, P_4^, P_5^}$ 是由学生网络[14]生成的一组特征图。

Benefiting from the additional supervision from the image-based model, the student network gets higher performance and takes fewer iterations to converge (Please see results in Table 5).

> 受到图像模型的额外监督，学生网络获得了更高的性能，并且需要更少的迭代才能收敛（请参见表5中的结果）。

## 3.5 Losses
The total loss of our approach is computed as:

$$
L = L_{cls} + L_{box} + \lambda_{dp} L_{dp} + \lambda_{kp} L_{kp} + \lambda_{tr} L_{tr}
$$


where $L_{cls}$, $L_{box}$, $L_{kp}$, $L_{Ltr}$ are losses for the person classification, bounding box regression, DensePose, keypoints, and transfer learning respectively. The classification loss $L_{cls}$ and the box regression loss $L_{box}$ are standard RCNN losses [9, 21]. The DensePose loss $L_{dp}$ [8] consists of several sub-components: (1) Cross-entropy loss for the coarse segmentation tasks. Each pixel is classified as either belonging to the background or one of the 24 human body regions. (2) Cross-entropy loss for body part classification and smooth L1 loss for UV coordinate regression. These losses are used to determine the exact coordinates of the pixels, i.e., 24 regressors are created to break the full human into small parts and parameterize each piece using a local two-dimensional UV coordinate system, that identifies the position UV nodes on this surface part.

> 其中，$L_{cls}$、$L_{box}$、$L_{kp}$、$L_{Ltr}$ 分别代表人员分类、边界框回归、DensePose、关键点以及转移学习的损失。分类损失 $L_{cls}$ 和边界框回归损失 $L_{box}$ 是标准的 RCNN 损失 [9,21]。DensePose 损失 $L_{dp}$ [8] 由几个子部分组成：(1)粗略分割任务的交叉熵损失。每个像素被分类为属于背景或 24 个人体区域之一。(2)身体部位分类的交叉熵损失和 UV 坐标回归的平滑 L1 损失。这些损失用于确定像素的精确坐标，即，创建 24 个回归器，将整个人分成小部分，并使用局部二维 UV 坐标系对每个部分进行参数化，该坐标系识别该表面部分的 UV 节点的位置。
![在这里插入图片描述](https://img-blog.csdnimg.cn/46b9b3784a8041d3be1bcb8e257aeb6d.png#pic_center)
*Figure 5: WiFi-DensePose RCNN. The 3 × 720 × 1280 feature map from Figure 4 first goes through standard ResNet-FPN and ROI pooling to extract person-wise features. The features are then processed by two heads:the Keypoint Head and the DensePose Head.*

> 图5：WiFi-DensePose RCNN。来自图4的3 × 720 × 1280特征映射首先经过标准的ResNet-FPN和ROI池化以提取人员特征。然后，这些特征被两个头部处理：关键点头和密集姿势头。

![在这里插入图片描述](https://img-blog.csdnimg.cn/0e1c2dd8fbe34bc7bb4723066e7459ed.png#pic_center)

*Figure 6: Transfer learning from an image-based teacher network to our WiFi-based network.*

> 图 6：从基于图像的教师网络转移到我们基于WiFi的网络。

We add $L_{kp}$ to help the DensePose to balance between the torso with more UV nodes and limbs with fewer UV nodes. Inspired by Keypoint RCNN [9], we one-hot-encode each of the 17 ground truth keypoints in one $56 \times 56$ heatmap, generating $17 \times 56 \times 56$ keypoints heatmaps and supervise the output with the Cross-Entropy Loss. To closely regularize the Densepose regression, the keypoint heatmap regressor takes the same input features used by the Denspose UV maps.

> 我们添加$L_{kp}$来帮助DensePose平衡具有更多UV节点的躯干和具有较少UV节点的四肢之间的平衡。受Keypoint RCNN [9]的启发，我们在一个$56 \times 56$的热图中对17个基础关键点进行一次热编码，生成$17 \times 56 \times 56$关键点热图并使用交叉熵损失监督输出。为了密切规范Densepose回归，关键点热图回归采用与Densepose UV图使用的相同输入特征。

# 4 EXPERIMENTS
This section presents the experimental validation of our WiFi-based DensePose.

## 4.1 Dataset
We used the dataset[^1] described in [31], which contains CSI samples taken at 100Hz from receiver antennas and videos recorded at 20 FPS. Time stamps are used to synchronize CSI and the video frames such that 5 CSI samples correspond to 1 video frame. The dataset was gathered in sixteen spatial layouts: six captures in the lab office and ten captures in the classroom. Each capture is around 13 minutes with 1 to 5 subjects (8 subjects in total for the entire dataset) performing daily activities under the layout described in Figure 2 (a). **The sixteen spatial layouts are different in their relative locations/orientations of the WiFi-emitter antennas, person, furniture, and WiFi-receiver antennas.**

> 我们使用了在[31]中描述的数据集，其中包含以100Hz的频率从接收天线获取的CSI样本和以20 FPS录制的视频。使用时间戳以同步CSI和视频帧，使得5个CSI样本对应于1个视频帧。该数据集在十六个空间布局中被收集：实验室办公室的六次捕捉和教室的十次捕捉。每次捕捉大约为13分钟，1至5个科目（整个数据集共8个科目）在图2（a）描述的布局下执行日常活动。十六种空间布局在WiFi发射天线，人，家具和WiFi接收天线的相对位置/方向不同。

There are no manual annotations for the data set. We use the MS-COCO-pre-trained dense model "R_101_FPN_s1x_legacy" [^2] and MS-COCO-pre-trained Keypoint R-CNN "R101-FPN" [^3] to pro- duce the pseudo ground truth. We denote the ground truth as "R101-Pseudo-GT" (see an annotated example in Figure 7). The R101-Pseudo-GT includes person bounding boxes, person-instance segmentation masks, body-part UV maps, and person-wise key- point coordinates.

> 这个数据集没有人工标注。我们使用以MS-COCO数据集预训练的密集模型 "R_101_FPN_s1x_legacy"[^2] 和 MS-COCO 预训练的关键点R-CNN "R101-FPN"[^3]生成伪地面实况。我们将地面实况称为 "R101-Pseudo-GT" (请参见图7中的注释示例)。R101-Pseudo-GT 包括人的边界框、人体实例分割掩码、身体部位的UV图和个人关键点坐标。

Throughout the section, we use R101-Puedo-GT to train our WiFi-based DensePose model as well as finetuning the image-based baseline "R_50_FPN_s1x_legacy".

> 在本节中，我们使用R101-Puedo-GT训练我们的基于WiFi的DensePose模型，并微调基于图像的基线"R_50_FPN_s1x_legacy"。

[^1]: The identifiable information in this dataset has been removed for any privacy concerns.
[^2]: https://github.com/facebookresearch/detectron2/blob/main/projects/DensePose/ doc/DENSEPOSE_IUV.md#ModelZoo
[^3]: https://github.com/facebookresearch/detectron2/blob/main/MODEL_ZOO.md# coco- person- keypoint- detection- baselines- with- keypoint- r- cnn


![在这里插入图片描述](https://img-blog.csdnimg.cn/e4e8cc99386c4dc2b9f22d0b14ba7b3d.png#pic_center)
*Figure 7: Top two rows are the amplitude and phase of the input WiFi signal. The bottom row shows R101-Psuedo-GT: the ground truth dense pose and keypoints annotated by running a pretrained image-based Densepose network on the corresponding RGB image (see Section 4.1 for details).*

> 图 7：顶部的两行是输入 WiFi 信号的振幅和相位。底行显示 R101-Pseudo-GT：用预先训练的基于图像的 DensePose 网络在相应 RGB 图像上运行后注释的密集姿态和关键点的真值（有关详细信息，请参见第 4.1 节）。

## 4.2 Training/Testing protocols and Metrics
We report results on two protocols: (1) Same layout: We train on the training set in all 16 spatial layouts, and test on remaining frames. Following [31], we randomly select 80% of the samples to be our training set, and the rest to be our testing set. The training and testing samples are different in the person’s location and pose, but share the same person’s identities and background. This is a reasonable assumption since the WiFi device is usually installed in a fixed location. (2) **Different layout**: We train on 15 spatial layouts and test on 1 unseen spatial layout. The unseen layout is in the classroom scenarios.

> 我们报告两种协议的结果：（1）相同布局：我们在所有16种空间布局的训练集上训练，并在剩余帧上进行测试。遵循[31]，我们随机选择80％的样本作为我们的训练集，其余样本作为我们的测试集。训练和测试样本在人的位置和姿态上不同，但共享相同的人的身份和背景。这是一个合理的假设，因为WiFi设备通常安装在固定位置。（2）不同布局：我们在15种空间布局上训练，在1个看不见的空间布局上测试。未见的布局在教室情景中。

We evaluate the performance of our algorithm in two aspects: the ability to detect humans (bounding boxes) and accuracy of the dense pose estimation.

> 我们评估算法在两方面的表现：检测人类（边界框）的能力和密集姿态估计的准确性。

To evaluate the performance of our models in detecting humans, we calculate the standard average precision (AP) of person bounding boxes at multiple IOU thresholds ranging from 0.5 to 0.95.

> 为了评估模型在检测人类方面的性能，我们计算人类边界框的标准平均精度（AP），其IOU阈值从0.5到0.95不等。

In addition, by MS-COCO [15] definition, we also compute AP-m for median bodies that are enclosed in bounding boxes with sizes between $32 \times 32$ and $96 \times 96$ pixels in a normalized $640 \times 480$ pixels image space, and AP-l for large bodies that are enclosed in bounding boxes larger than $96 \times 96$ pixels.

> 此外，根据 MS-COCO [15] 的定义，我们还计算 AP-m，这是指以 $32 \times 32$ 到 $96 \times 96$ 像素的大小包含在 $640 \times 480$ 像素的归一化图像空间中的边界框中的中位数身体；AP-l 是指边界框大于 $96 \times 96$ 像素的大型身体。

To measure the performance of DensePose detection, we follow the original DensePose paper [8]. We first compute Geodesic Point Similarity (GPS) as a matching score for dense correspondences:

> 为了衡量 DensePose 检测的性能，我们遵循 DensePose 原始论文 [8]。我们首先计算几何点相似性（GPS）作为密集对应的匹配分数：

**E.q. 4**

$$
GPS_{j} = \frac{1}{| P_j |} \sum_{p \in P_j} \exp (\frac{ -g (i_p, \hat{i}_p)^2 }{2 k^2})
$$

where $g$ calculates the geodesic distance, $P_j$ denotes the ground truth point annotations of person $j$, $i_p$ and $\hat{i}_p$ are the estimated and ground truth vertex at point $p$ respectively, and $k$ is a normalizing parameter (set to be 0.255 according to [8]).

> $g$ 计算几何距离，$P_j$ 表示人 $j$ 的地面真实点注释，$i_p$ 和 $\hat{i}_p$ 分别是点 $p$ 的估计值和地面实况值，$k$ 是归一化参数（根据 [8] 设为 0.255）。

One issue of GPS is that it does not penalize spurious predictions. Therefore, estimations with all pixels classified as foreground are favored. To alleviate this issue, masked geodesic point similarity (GPSm) was introduced in [8], which incorporates both GPS and segmentation masks. The formulation is as follows:

> GPS 的一个问题是它不惩罚误报预测。因此，将所有像素分类为前景的估计值受到青睐。为了减轻这个问题，在 [8] 中引入了带掩码的几何点相似度（GPSm），它结合了 GPS 和分割掩码。公式如下：

**E.q. 5**

$$
GPSm = \sqrt{GPS \cdot I}, I = \frac{M \cap \hat M}{M \cup \hat M}
$$

where $M$ and $\hat M$ are the predicted and ground truth foreground segmentation masks.

> 其中 $M$ 和 $\hat M$ 是预测的和真实前景分割掩码。

Next, we can calculate DensePose average precision with GPS (denoted as $dpAP \cdot GPS$) and GPSm (denoted as $dpAP \cdot GPSm$) as thresholds, following the same logic behind the calculation of bounding box AP.

> 接下来，我们可以使用GPS（表示为$dpAP \cdot GPS$）和GPSm（表示为$dpAP \cdot GPSm$）作为阈值计算DensePose平均精度，遵循与计算边界框AP相同的逻辑。

## 4.3 Implementation Details

We implemented our approach in PyTorch. We set the training batch size to 16 on a 4 GPU (Titan X) server. We empirically set $\lambda_{dp} = 0.6$, $\lambda_{kp} = 0.3$, $\lambda_{tr} = 0.1$. We used a warmup multi-step learning rate scheduler and set the initial learning rate as $1e − 5$. The learning rate increases to $1e − 3$ during the first 2000 iterations then decreases to $\frac{1}{10}$ of its value every 48000 iterations. We trained for 145, 000 iterations for our final model.

> 我们使用 PyTorch 实现了我们的方法。我们在 4 GPU（Titan X）服务器上将训练批次设置为 16。我们经验证地设置了 $\lambda_{dp} = 0.6$，$\lambda_{kp} = 0.3$，$\lambda_{tr} = 0.1$。我们使用了一个热身多步学习率调度器，并将初始学习率设为 $1e − 5$。学习率在前 2000 次迭代中增加到 $1e − 3$，然后每 48000 次迭代减少到其值的 $\frac{1}{10}$。我们为最终模型训练了 145,000 次迭代。

## 4.4 WiFi-based DensePose under Same Layout
Under the Same Layout protocol, we compute the AP of human bounding box detections as well as $dpAP \cdot GPS$ and $dpAP \cdot GPSm$ of dense correspondence predictions. Results are presented in Table 1 and Table 2, respectively.

> 在“同一布局”协议下，我们计算人体边界框检测的AP以及密集对应预测的 $dpAP \cdot GPS$和 $dpAP \cdot GPSm$。结果分别在表1和表2中呈现。

![在这里插入图片描述](https://img-blog.csdnimg.cn/04e4ed02428943cb85ea73161e0b5c41.png#pic_center)
*Table 1: Average precision (AP) of WiFi-based DensePose un- der the Same Layout protocol. All metrics are the higher the better.*

> 表1：在同一布局协议下WiFi基础密集姿态的平均精度（AP）。所有指标越高越好。

From Table 1, we can observe a high value (87.2) of AP@50, indicating that our model can effectively detect the approximate locations of human bounding boxes. The relatively low value (35.6) for AP@75 suggests that the details of the human bodies are not perfectly estimated.

> 从表1可以看出，AP@50的值较高（87.2），表明我们的模型可以有效地检测人体边界框的大致位置。 AP@75的相对较低的值（35.6）表明人体的细节没有得到完美的估计。

![在这里插入图片描述](https://img-blog.csdnimg.cn/41866f5e740b44b0a91e7942bc6184f9.png#pic_center)

*Table 2: DensePose Average precision (dpAP $\cdot$ GPS, dpAP $\cdot$ GPSm) of WiFi-based DensePose under the Same Layout protocol. All metrics are the higher the better.*

> 表2：WiFi基础DensePose在同一布局协议下的密集对应的平均精度（dpAP·GPS，dpAP·GPSm）。所有指标越高越好。

A similar pattern can be seen from the results of DensePose estimations (see Table 2 for details). Experiments report high values of dpAP $\cdot$ GPS@50 and dpAP $\cdot$ GPSm@50, but low values of dpAP $\cdot$ GPS@75 and dpAP $\cdot$ GPSm@75. This demonstrates that our model performs well at estimating the poses of human torsos, but still struggles with detecting details like limbs.

> 从结果中也可以看出类似的模式（详情请见表2）。实验报告了高的dpAP $\cdot$ GPS@50和dpAP $\cdot$ GPSm@50值，但dpAP $\cdot$ GPS@75和dpAP $\cdot$ GPSm@75的值较低。这表明我们的模型在估计人体躯干的姿态方面表现良好，但仍然在检测像四肢这样的细节方面存在困难。

## 4.5 Comparison with Image-based DensePose

![在这里插入图片描述](https://img-blog.csdnimg.cn/000533281df7454ba5a3e2ca13347989.png#pic_center)
*Table 3: Average precision (AP) of WiFi-based and image- based DensePose under the Same Layout protocol. All met- rics are the higher the better.*

> 表3：在相同布局协议下WiFi基础和图像基础的DensePose的平均精度（AP）。所有指标越高越好。

As discussed in Section 4.1, since there are no manual annota- tions on the WiFi dataset, it is difficult to compare the performance of WiFi-based DensePose with its Image-based counterpart. This is a common drawback of many WiFi perception works including [31].

> 如4.1节中所讨论的，由于WiFi数据集上没有手动注释，因此很难将WiFi基础的DensePose与其基于图像的对应物进行比较。这是许多WiFi感知工作的常见缺点，包括[31]。

Nevertheless, conducting such a comparison is still worthwhile in assessing the current limit of WiFi-based perception. We tried an image-based DensePose baseline "R_50_FPN_s1x_legacy" finetuned on R101-Pseudo-GT under the Same Layout protocol. In addition, as shown in Figure 9 and Figure 10, though certain defects still exist, the estimations from our WiFi-based model are reasonably well compared to the results produced by Image-based DensePose.

> 尽管如此，进行这样的比较仍然有意义，因为它可以评估WiFi基础感知的当前限制。我们试图在Same Layout协议下对基于图像的DensePose基线"R_50_FPN_s1x_legacy"进行微调，该基线在R101-Pseudo-GT上进行了微调。此外，如图9和图10所示，尽管仍存在某些缺陷，但我们的基于WiFi的模型的估计与基于图像的DensePose的结果相比是合理的。

In the quantitative results in Table 3 and Table 4, the image-based baseline produces very high APs due to the small difference between its ResNet50 backbone and the Resnet101 backbone used to generate R101-Pseudo-GT. This is to be expected. Our WiFi-based model has much lower absolute metrics. However, it can be observed from Table 3 that the difference between AP-m and AP-l values is relatively small for the WiFi-based model. We believe this is because individuals who are far away from cameras occupy less space in the image, which leads to less information about these subjects. On the contrary, WiFi signals incorporate all the information in the entire scene, regardless of the subjects’ locations.

> 在表3和表4的定量结果中，基于图像的基线由于其ResNet50骨干与用于生成R101-Pseudo-GT的Resnet101骨干的差异很小，因此产生了非常高的AP。这是可以预期的。我们的基于WiFi的模型具有更低的绝对指标。然而，从表3可以看出，WiFi模型的AP-m和AP-l值之间的差异相对较小。我们相信这是因为远离摄像头的个体占用图像空间较小，导致对这些对象信息较少。相反，WiFi信号包含整个场景中的所有信息，而不考虑主体的位置。

## 4.6 Ablation Study

This section describes the ablation study to understand the effects of phase information, keypoint supervision, and transfer learning on estimating dense correspondences. Similar to section 4.4, the models analyzed in this section are all trained under the same-layout mentioned in section 4.2.

> 这一节描述了一个消融研究，以了解相位信息、关键点监督和转移学习对估计密集对应的影响。与第4.4节类似，这一节分析的模型均在第4.2节中所述的相同布局下训练。

We start by training a baseline WiFi model, which does not in- clude the phase encoder, the keypoint detection branch, or transfer learning. The results are presented in the first row of both Table 5 and Table 6 as a reference.

> 我们首先训练一个基线WiFi模型，它不包括相位编码器、关键点检测分支或转移学习。结果在表5和表6的第一行呈现，作为参考。

**Addition of Phase information:** We first examine whether the phase information can enhance the baseline performance. As shown in the second row of Table 5 and Table 6, the results for all the metrics have slightly improved from the baseline. This proves our hypothesis that the phase can reveal relevant information about the dense human pose.

> **相位信息的加入**: 我们首先检查相位信息是否能增强基线表现。如表5和表6的第二行所示，所有度量的结果都从基线有所提高。这证明了我们的假设，即相位可以揭示关于密集人体姿势的相关信息。

**Addition of a keypoint detection branch**: Having established the advantage of incorporating phase information, we now evaluate the effect of adding a keypoint branch to our model. The quantita- tive results are summarized in the third row of Table 5 and Table 6.

> **加入关键点检测分支**: 在证明了结合相位信息的优势之后，我们现在评估加入关键点分支对我们的模型的影响。定量结果总结在表5和表6的第三行中。

Comparing with the numbers on the second row, we can observe a slight increase in performance in terms of dpAP $\cdot$ GPS@50(from 77.4 to 78.8) and dpAP $\cdot$ GPSm@50 (from 75.7 to 76.8), and a more noticeable improvement in terms of dpAP $\cdot$ GPS@75 (from 42.3 to 46.9) and dpAP $\cdot$ GPSm@75 (from 40.5 to 44.9). This indicates that the keypoint branch provides effective references to dense pose estimations, and our model becomes significantly better at detecting subtle details (such as the limbs).

> 对比第二行的数字，我们可以观察到在 dpAP $\cdot$ GPS@50（从77.4提高到78.8）和dpAP $\cdot$ GPSm@50（从75.7提高到76.8）方面的性能略微有所提高，在dpAP $\cdot$ GPS@75（从42.3提高到46.9）和dpAP $\cdot$ GPSm@75（从40.5提高到44.9）方面的提高则更加显著。这表明，关键点分支为密集姿势估计提供了有效的参考，我们的模型在检测细微的细节（如四肢）方面变得更好。

**Effect of Transfer Learning**: We aim to reduce the training time for our model with the help of transfer learning. For each model in Table 5, we continue training the model until there are no significant changes in terms of performance. The last row of Table 5 and Table 6 represents our final model with transfer learn- ing. Though the final performance does not improve too much compared to the model (with phase information and keypoints) without transfer learning, it should be noted that the number of training iterations decreases significantly from 186000 to 145000 (this number includes the time to perform transfer learning as well as training the main model).

> **转移学习的效果**: 我们希望借助转移学习来缩短我们模型的训练时间。对于表5中的每个模型，我们继续训练模型，直到在性能方面没有显著的变化。表5和表6的最后一行代表我们的最终模型，进行转移学习。虽然与不进行转移学习的模型（带有相位信息和关键点）相比，最终的表现没有太大的改善，但是应该注意的是，训练次数从186000显着减少到145000（这个数字包括执行转移学习和训练主模型的时间）。

## 4.7 Performance in different layouts

All above results are obtained using the same layout for training and testing. However, WiFi signals in different environments exhibit significantly different propagation patterns. Therefore, it is still a very challenging problem to deploy our model on data from an untrained layout.

> 所有上述结果都是在使用相同的布局进行训练和测试时获得的。但是，在不同环境中的WiFi信号表现出显著不同的传播模式。因此，在未经训练的布局的数据上部署我们的模型仍然是一个非常具有挑战性的问题。

To test the robustness of our model, we conducted the previous experiment under the different layout protocols, where there are 15 training layouts and 1 testing layout. The experimental results are recorded in Table 7 and Table 8.

> 为了测试我们模型的鲁棒性，我们在不同的布局协议下进行了前面的实验，其中有15个训练布局和1个测试布局。实验结果记录在表7和表8中。

We can observe that our final model performs better than the baseline model in the unseen domain, but the performance decreases significantly from that under the same layout protocol: the AP performance drops from 43.5 to 27.3 and dpAP $\cdot$ GPS drops from 45.3 to 25.4. However, it should also be noted that the image-based model suffers from the same domain generalization problem. We believe a more comprehensive dataset from a wide range of settings can alleviate this issue.

> 我们可以发现，我们的最终模型在看不见的领域中的表现比基线模型好，但是性能仍然明显下降：AP性能从43.5降到27.3，dpAP $\cdot$ GPS从45.3降到25.4。但是，应该也要注意的是，基于图像的模型也存在相同的领域泛化问题。我们相信来自各种设置的更全面的数据集可以缓解这个问题。

![在这里插入图片描述](https://img-blog.csdnimg.cn/51713ddffd7e40c786a9403fb61ae3d5.png#pic_center)
*Table 4: DensePose Average precision (dpAP $\cdot$ GPS, dpAP $\cdot$ GPSm) of WiFi-based and image-based DensePose under the Same Layout protocol. All metrics are the higher the better.*

> *表4：相同布局协议下基于WiFi和基于图像的DensePose的平均精度（dpAP $\cdot$ GPS，dpAP $\cdot$ GPSm）。所有指标都是越高越好*。

![在这里插入图片描述](https://img-blog.csdnimg.cn/b858a85a99c8447987d41ca8a99cba5f.png#pic_center)

*Table 5: Ablation study of human detection under the Same-layout protocol. All metrics are the higher the better.*

> *表5：相同布局方案下的人类检测的消融研究。所有指标都是越高越好*。

![在这里插入图片描述](https://img-blog.csdnimg.cn/3eab382cd9a5478698793ee0a876a0dc.png#pic_center)
*Table 6: Ablation study of DensePose estimation under the Same-layout protocol. All metrics are the higher the better.*

> *表6：相同布局协议下的DensePose估计的消融研究。所有指标都是越高越好*。

![在这里插入图片描述](https://img-blog.csdnimg.cn/927f16c65e624970bfa3a64f085aca94.png#pic_center)
*Table 7: Average precision (AP) of WiFi-based and image-based DensePose under the Different Layout protocol. All metrics are the higher the better.*

> *表7：不同布局协议下基于WiFi和基于图像的DensePose的平均精度（AP）。所有指标都是越高越好*。

![在这里插入图片描述](https://img-blog.csdnimg.cn/6ad7c637bc894d0a940ae6785f0657a7.png#pic_center)
*Table 8: DensePose Average precision (dpAP $\cdot$ GPS, dpAP $\cdot$ GPSm) of WiFi-based and image-based DensePose under the Differ- ent Layout protocol. All metrics are the higher the better.*

> *表8：DensePose 在Differ-ent Layout协议下，基于WiFi和基于图像的DensePose的平均精度（dpAP $\cdot$ GPS, dpAP $\cdot$ GPSm）。所有指标都是越高越好*。

## 4.8 Failure cases
We observed two main types of failure cases. (1) When there are body poses that rarely occurred in the training set, the WiFi-based model is biased and is likely to produce wrong body parts (See examples (a-b) in Figure 8). (2) When there are three or more concurrent subjects in one capture, it is more challenging for the WiFi-based model to extract detailed information for each individual from the amplitude and phase tensors of the entire capture. (See examples (c-d) in Figure 8). We believe both of these issues can be resolved by obtaining more comprehensive training data.

> 我们观察到了两种主要的失败情况。（1）当训练集中很少出现身体姿势时，WiFi基于的模型存在偏见，可能会产生错误的身体部分（请参见图8（a-b）中的示例）。（2）当一次捕捉中存在三个或三个以上的并发主题时，WiFi基于的模型从整个捕捉的幅值和相位张量中提取每个个体的详细信息更具挑战性（请参见图8（c-d）中的示例）。我们相信这两个问题都可以通过获得更全面的训练数据来解决。

# 5 CONCLUSION AND FUTURE WORK
In this paper, we demonstrated that it is possible to obtain dense human body poses from WiFi signals by utilizing deep learning architectures commonly used in computer vision. Instead of directly training a randomly initialized WiFi-based model, we explored rich supervision information to improve both the performance and training efficiency, such as utilizing the CSI phase, adding keypoint detection branch, and transfer learning from an image-based model. The performance of our work is still limited by the public training data in the field of WiFi-based perception, especially under different layouts. In future work, we also plan to collect multi-layout data and extend our work to predict 3D human body shapes from WiFi signals. We believe that the advanced capability of dense perception could empower the WiFi device as a privacy-friendly, illumination- invariant, and cheap human sensor compared to RGB cameras and Lidars.

> 本文证明了通过利用计算机视觉领域中常用的深度学习架构，可以从WiFi信号中获得密集的人体姿势。我们不是直接训练一个随机初始化的WiFi模型，而是探索了丰富的监督信息来提高性能和训练效率，例如利用CSI相位，添加关键点检测分支，以及从基于图像的模型进行转移学习。我们的工作仍然受到WiFi感知领域的公共训练数据的限制，特别是在不同的布局下。在未来的工作中，我们还计划收集多布局数据，并将我们的工作扩展到预测WiFi信号中的3D人体形状。我们相信密集感知的先进能力可以使WiFi设备成为隐私友好，不受照明影响，并且比RGB摄像头和Lidar便宜的人体传感器。

![在这里插入图片描述](https://img-blog.csdnimg.cn/9fa64117a77049a0884c33e96d30483e.png#pic_center)
*Figure 8: Examples pf failure cases: (a-b) rare poses; (c-d) Three or more concurrent subjects. The first row is ground truth dense pose estimation. The second row illustrates the predicted dense pose.*

> *图8：失败案例的例子：(a-b)罕见的姿势；(c-d)三个或更多同时出现的主体。第一行是地面真实的密集姿态估计。第二行说明了预测的密集姿态。
![在这里插入图片描述](https://img-blog.csdnimg.cn/3e985f2389044ed5a6bfd3b89cb9932c.png#pic_center)
*Figure 9: Qualitative comparison using synchronized images and WiFi signals. (Left Column) image-based DensePose (Right Column) our WiFi-based DensePose.*

> *图9：使用同步图像和WiFi信号的定性比较。(左栏）基于图像的DensePose（右栏）我们基于WiFi的DensePose。

![在这里插入图片描述](https://img-blog.csdnimg.cn/10f2b9e5465b4f53890feb65cdb3bce8.png#pic_center)

*Figure 10: More qualitative comparison using synchronized images and WiFi signals. (Left Column) image-based DensePose (Right Column) our WiFi-based DensePose.*

> *图10：使用同步的图像和WiFi信号进行更多的定性比较。(左栏）基于图像的DensePose（右栏）我们基于WiFi的DensePose。

# REFERENCES

[1] Fadel Adib, Chen-Yu Hsu, Hongzi Mao, Dina Katabi, and Frédo Durand. 2015. Capturing the Human Figure through a Wall. ACM Trans. Graph. 34, 6, Article 219 (oct 2015), 13 pages. https://doi.org/10.1145/2816795.2818072
[2] FadelAdib,ZachKabelac,DinaKatabi,andRobertC.Miller.2014.3DTrackingvia Body Radio Reflections. In 11th USENIX Symposium on Networked Systems Design andImplementation(NSDI14).USENIXAssociation,Seattle,WA,317–329. https: //www.usenix.org/conference/nsdi14/technical-sessions/presentation/adib
[3] Thiemo Alldieck, Gerard Pons-Moll, Christian Theobalt, and Marcus Magnor. 2019. Tex2Shape: Detailed Full Human Body Geometry From a Single Image. arXiv:1904.08645 [cs.CV]
[4] MykhayloAndriluka,UmarIqbal,EldarInsafutdinov,LeonidPishchulin,Anton Milan, Juergen Gall, and Bernt Schiele. 2018. PoseTrack: A Benchmark for Human Pose Estimation and Tracking. arXiv:1710.10000 [cs.CV]
[5] Chaimaa BASRI and Ahmed El Khadimi. 2016. Survey on indoor localization system and recent advances of WIFI fingerprinting technique. In International Conference on Multimedia Computing and Systems.
[6] Hilton Bristow, Jack Valmadre, and Simon Lucey. 2015. Dense Semantic Corre- spondence where Every Pixel is a Classifier. arXiv:1505.04143 [cs.CV]
[7] Zhe Cao, Gines Hidalgo, Tomas Simon, Shih-En Wei, and Yaser Sheikh. 2019. OpenPose: Realtime Multi-Person 2D Pose Estimation using Part Affinity Fields. arXiv:1812.08008 [cs.CV]
[8] Riza Alp Güler, Natalia Neverova, and Iasonas Kokkinos. 2018. DensePose: Dense Human Pose Estimation In The Wild. CoRR abs/1802.00434 (2018). arXiv:1802.00434 http://arxiv.org/abs/1802.00434
[9] Kaiming He, Georgia Gkioxari, Piotr Dollár, and Ross B. Girshick. 2017. Mask R-CNN. CoRR abs/1703.06870 (2017). arXiv:1703.06870 http://arxiv.org/abs/1703. 06870
[10] Weipeng Jiang, Yongjun Liu, Yun Lei, Kaiyao Wang, Hui Yang, and Zhihao Xing. 2017. For Better CSI Fingerprinting Based Localization: A Novel Phase Sanitization Method and a Distance Metric. In 2017 IEEE 85th Vehicular Technology Conference(VTCSpring).1–7. https://doi.org/10.1109/VTCSpring.2017.8108351
[11] Mohammad Hadi Kefayati, Vahid Pourahmadi, and Hassan Aghaeinia. 2020. Wi2Vi: Generating Video Frames from WiFi CSI Samples. CoRR abs/2001.05842 (2020). arXiv:2001.05842 https://arxiv.org/abs/2001.05842
[12] Muhammed Kocabas, Nikos Athanasiou, and Michael J. Black. 2020. VIBE: Video Inference for Human Body Pose and Shape Estimation. arXiv:1912.05656 [cs.CV]
[13] Anton Ledergerber and Raffaello D’Andrea. 2019. Ultra-Wideband Angle of Ar- rival Estimation Based on Angle-Dependent Antenna Transfer Function. Sensors 19, 20 (2019). https://doi.org/10.3390/s19204466
[14] Tsung-YiLin,PiotrDollár,RossB.Girshick,KaimingHe,BharathHariharan,and Serge J. Belongie. 2016. Feature Pyramid Networks for Object Detection. CoRR
abs/1612.03144 (2016). arXiv:1612.03144 http://arxiv.org/abs/1612.03144
[15] Tsung-YiLin,MichaelMaire,SergeJ.Belongie,LubomirD.Bourdev,RossB.Gir- shick, James Hays, Pietro Perona, Deva Ramanan, Piotr Dollár, and C. Lawrence
Zitnick. 2014. Microsoft COCO: Common Objects in Context. CoRR abs/1405.0312
[16] Jonathan Long, Evan Shelhamer, and Trevor Darrell. 2014. Fully Convo- lutional Networks for Semantic Segmentation. CoRR abs/1411.4038 (2014). arXiv:1411.4038 http://arxiv.org/abs/1411.4038
[17] Daniel Maturana and Sebastian Scherer. 2015. VoxNet: A 3D Convolutional Neural Network for real-time object recognition. In 2015 IEEE/RSJ International Conference on Intelligent Robots and Systems (IROS). 922–928. https://doi.org/10. 1109/IROS.2015.7353481
[18] NataliaNeverova,DavidNovotny,VasilKhalidov,MarcSzafraniec,PatrickLa- batut, and Andrea Vedaldi. 2020. Continuous Surface Embeddings. Advances in Neural Information Processing Systems.
[19] Joseph Redmon and Ali Farhadi. 2018. YOLOv3: An Incremental Improvement. arXiv:1804.02767 [cs.CV]
[20] Shaoqing Ren, Kaiming He, Ross B. Girshick, and Jian Sun. 2015. Faster R-CNN: Towards Real-Time Object Detection with Region Proposal Networks. CoRR abs/1506.01497 (2015). arXiv:1506.01497 http://arxiv.org/abs/1506.01497
[21] Shaoqing Ren, Kaiming He, Ross B. Girshick, and Jian Sun. 2015. Faster R-CNN: Towards Real-Time Object Detection with Region Proposal Networks. CoRR abs/1506.01497 (2015). arXiv:1506.01497 http://arxiv.org/abs/1506.01497
[22] Shunsuke Saito, , Zeng Huang, Ryota Natsume, Shigeo Morishima, Angjoo Kanazawa, and Hao Li. 2019. PIFu: Pixel-Aligned Implicit Function for High- Resolution Clothed Human Digitization. arXiv preprint arXiv:1905.05172 (2019).
[23] SouvikSen,BožidarRadunovic,RomitRoyChoudhury,andTomMinka.2012. You Are Facing the Mona Lisa: Spot Localization Using PHY Layer Information. In Proceedings of the 10th International Conference on Mobile Systems, Applications, and Services (Low Wood Bay, Lake District, UK) (MobiSys ’12). Association for Computing Machinery, New York, NY, USA, 183–196. https://doi.org/10.1145/ 2307636.2307654
[24] RomanShapovalov,DavidNovotný,BenjaminGraham,PatrickLabatut,andAn- drea Vedaldi. 2021. DensePose 3D: Lifting Canonical Surface Maps of Articulated Objects to the Third Dimension. CoRR abs/2109.00033 (2021). arXiv:2109.00033 https://arxiv.org/abs/2109.00033
[25] B. Sheng, F. Xiao, L. Sha, and L. Sun. 2020. Deep Spatial–Temporal Model Based Cross-Scene Action Recognition Using Commodity WiFi. IEEE Internet of Things Journal 7 (2020).
[26] Elahe Soltanaghaei, Avinash Kalyanaraman, and Kamin Whitehouse. 2018. Mul- tipath Triangulation: Decimeter-Level WiFi Localization and Orientation with a Single Unaided Receiver. In Proceedings of the 16th Annual International Con- ference on Mobile Systems, Applications, and Services (Munich, Germany) (Mo- biSys ’18). Association for Computing Machinery, New York, NY, USA, 376–388. https://doi.org/10.1145/3210240.3210347
[27] Elahe Soltanaghaei, Avinash Kalyanaraman, and Kamin Whitehouse. 2018. Mul- tipath Triangulation: Decimeter-level WiFi Localization and Orientation with a Single Unaided Receiver. In Proceedings of the 16th Annual International Confer- ence on Mobile Systems, Applications, and Services.
[28] YuSun,QianBao,WuLiu,YiliFu,MichaelJ.Black,andTaoMei.2021.Monocular, One-stage, Regression of Multiple 3D People. arXiv:2008.12272 [cs.CV]
[29] FeiWang,YunpengSong,JimuyangZhang,JinsongHan,andDongHuang.2019. Temporal Unet: Sample Level Human Action Recognition using WiFi. arXiv Preprint arXiv:1904.11953 (2019).
[30] FeiWang,SanpingZhou,StanislavPanev,JinsongHan,andDongHuang.2019. Person-in-WiFi: Fine-grained Person Perception using WiFi. CoRR abs/1904.00276 (2019). arXiv:1904.00276 http://arxiv.org/abs/1904.00276
[31] Fei Wang, Sanping Zhou, Stanislav Panev, Jinsong Han, and Dong Huang. 2019. Person-in-WiFi: Fine-grained Person Perception using WiFi. In ICCV.
[32] Zhe Wang, Yang Liu, Qinghai Liao, Haoyang Ye, Ming Liu, and Lujia Wang. 2018. Characterization of a RS-LiDAR for 3D Perception. In 2018 IEEE 8th Annual International Conference on CYBER Technology in Automation, Control, and Intelligent Systems (CYBER). 564–569. https://doi.org/10.1109/CYBER.2018.8688235
[33] Shih-En Wei, Varun Ramakrishna, Takeo Kanade, and Yaser Sheikh. 2016. Convolutional Pose Machines. arXiv:1602.00134 [cs.CV]
[34] Bin Xiao, Haiping Wu, and Yichen Wei. 2018. Simple Baselines for Human Pose Estimation and Tracking. arXiv:1804.06208 [cs.CV]
[35] Yuanlu Xu, Song-Chun Zhu, and Tony Tung. 2019. DenseRaC: Joint 3D Pose and Shape Estimation by Dense Render-and-Compare. CoRR abs/1910.00116 (2019). arXiv:1910.00116 http://arxiv.org/abs/1910.00116
[36] Hongfei Xue, Yan Ju, Chenglin Miao, Yijiang Wang, Shiyang Wang, Aidong Zhang, and Lu Su. 2021. mmMesh: towards 3D real-time dynamic human mesh con- struction using millimeter-wave. In Proceedings of the 19th Annual International Conference on Mobile Systems, Applications, and Services. 269–282.
[37] Pengfei Yao, Zheng Fang, Fan Wu, Yao Feng, and Jiwei Li. 2019. DenseBody: Directly Regressing Dense 3D Human Pose and Shape From a Single Color Image. arXiv:1903.10153 [cs.CV]
[38] Hongwen Zhang, Jie Cao, Guo Lu, Wanli Ouyang, and Zhenan Sun. 2020. Learning 3D Human Shape and Pose from Dense Body Parts. arXiv:1912.13344 [cs.CV]
[39] Mingmin Zhao, Tianhong Li, Mohammad Abu Alsheikh, Yonglong Tian, Hang Zhao, Antonio Torralba, and Dina Katabi. 2018. Through-Wall Human Pose Estimation Using Radio Signals. In 2018 IEEE/CVF Conference on Computer Vision and Pattern Recognition. 7356–7365. https://doi.org/10.1109/CVPR.2018.00768
[40] Tinghui Zhou, Philipp Krähenbühl, Mathieu Aubry, Qixing Huang, and Alexei A. Efros. 2016. Learning Dense Correspondence via 3D-guided Cycle Consistency. arXiv:1604.05383 [cs.CV]
