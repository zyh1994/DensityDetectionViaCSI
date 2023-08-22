 **Table of content:**
- [Bibliography](#bibliography)
- [Abstract](#abstract)
- [1 Introduction](#1-introduction)
- [2 Related works](#2-related-works)
  - [2.1 Human behavior detection with CSI](#21-human-behavior-detection-with-csi)
  - [2.2 Novel applications based on CSI](#22-novel-applications-based-on-csi)
  - [2.3 CSI data collection](#23-csi-data-collection)
  - [2.4 CSI feature extraction](#24-csi-feature-extraction)
- [3. PCA-Kalman design](#3-pca-kalman-design)
  - [3.1 Offline training phase](#31-offline-training-phase)
  - [3.2 Online behavior testing phase](#32-online-behavior-testing-phase)
- [4 Experimental setup](#4-experimental-setup)
  - [4.1 Hardware testbed](#41-hardware-testbed)
  - [4.2 Experimental scenarios](#42-experimental-scenarios)
- [5 Numerical results and discussion](#5-numerical-results-and-discussion)
  - [5.1 Feasibility of PCA-Kalman](#51-feasibility-of-pca-kalman)
  - [5.2 Robustness verification](#52-robustness-verification)
  - [5.3 The effect of different test areas on the detection](#53-the-effect-of-different-test-areas-on-the-detection)
  - [5.4 Detection of different performance areas](#54-detection-of-different-performance-areas)
  - [5.5 Impact of different sliding window sizes](#55-impact-of-different-sliding-window-sizes)
  - [5.6 Impact of feature number](#56-impact-of-feature-number)
  - [5.7 Impact of packet number](#57-impact-of-packet-number)
- [6 Conclusions](#6-conclusions)
- [References](#references)

 <a id="Bibliography"></a>
# Bibliography

Dang X, Huang Y, Hao Z, et al. PCA-Kalman: device-free indoor human behavior detection with commodity Wi-Fi[J]. EURASIP Journal on Wireless Communications and Networking, 2018, 2018: 1-17.

<a id="Abstract"></a>
# Abstract

Open Access Human behavior detection has become increasingly significant in various fields of application. In this paper, we propose a device-free indoor human behavior detection method with channel state information (CSI) and principal component analysis (PCA), respectively, in the line of sight environment, non-line-of-sight environment, and through the wall environment experiments. 

> 开放获取人类行为检测在各种应用领域中变得越来越重要。在本文中，我们分别在视线环境、非视线环境和穿墙环境实验中提出了一种基于信道状态信息（CSI）和主成分分析（PCA）的无设备室内人类行为检测方法。

We divide this method into two parts. It begins with an online phase. A fingerprint database is established by collecting the original data packets of CSI in different time periods and using the characteristics of PCA algorithm to reduce the dimension of the original CSI data. Then, some outlier values are removed by Kalman filter algorithm, and we will get more stable data and fully prepared for the docking experiments. 

> 我们将此方法分为两部分。首先是在线阶段。通过收集不同时间段的CSI原始数据包，并使用PCA算法的特性来减少原始CSI数据的维度，建立了一个指纹数据库。然后，通过卡尔曼滤波算法去除一些异常值，我们将获得更稳定的数据，并为对接实验做好充分准备。

At the same time, the PCA algorithm’s estimation results are corrected according to the detected real-time motion speed to reduce the mismatch target. Then, in the offline phase, the classification of data is collected in the real-time environment by using support vector machine (SVM) algorithm. 

> 同时，根据检测到的实时运动速度校正PCA算法的估算结果，以减少目标不匹配。然后，在离线阶段，使用支持向量机（SVM）算法收集实时环境中的数据分类。

This method not only reduces the time complexity of the algorithm but also improves the detection rate of the human’s behavior and reduces the error. The processed data are matched with the data in the fingerprint database, and finally, the detection of different behaviors performed by humans in an indoor environment is finally achieved according to the matching results. 

> 这种方法不仅减少了算法的时间复杂性，还提高了人类行为的检测率并减少了错误。处理过的数据与指纹数据库中的数据匹配，最终根据匹配结果实现室内环境中人类执行的不同行为的检测。

We experimented repeatedly in three different scenarios, with an average 95% of human behavior detection rate in three different environments. In addition, we compare the method proposed in this paper with the existing methods in different aspects, such as the impact of the number of subcarriers, the impact of data packets, and the impact of the test area. 

> 我们在三种不同的场景中进行了反复实验，三种不同环境中的人类行为检测率平均为95%。此外，我们比较了本文提出的方法与现有方法在不同方面的影响，如子载波数量的影响、数据包的影响和测试区域的影响。

The experimental results show that this method is superior to other algorithms in terms of average error and indoor activity recognition accuracy, which can more accurately identify indoor human motion behavior and improve the stability of the system.

> 实验结果表明，这种方法在平均误差和室内活动识别准确性方面优于其他算法，可以更准确地识别室内人体运动行为并提高系统的稳定性。

**Keywords**: Principal component analysis, Human behavior detection, Channel state information, Support vector machine, Kalman filter

<a id="1_Introduction"></a>
# 1 Introduction

Detecting human behavior has aroused increasing attention in many potential applications, such as indoor intrusion detection, campus security, supermarket customer tracking, patient care, and elderly safety monitoring.

> 探测人类行为在许多潜在的应用中引起了越来越多的关注，例如室内入侵检测、校园安全、超市客户跟踪、病人护理和老年人安全监测。

With the continuous progress and development of wireless sensor networks (WSNs), people’s research perspective has not only limited to the traditional location awareness and indoor location [1]. For example, the typical ultra-wideband (UWB)-based radar system [2] and the relatively new indoor positioning technology based on commercial [3] Wi-Fi infrastructure have better development advan- tages in all aspects [4].

> 随着无线传感器网络（WSNs）的持续进展和发展，人们的研究视角不仅局限于传统的位置感知和室内定位[1]。例如，典型的基于超宽带（UWB）的雷达系统[2]和基于商用[3] Wi-Fi基础设施的相对较新的室内定位技术在各个方面都有更好的发展优势[4]。

A common solution to these problems is to pinpoint mobile devices in a Wi-Fi infrastructure [5]. Such a solution requires the active participation of mobile devices [6]. However, in many special occasions, most people do not carry equipment at any time. Such as at home or in sensitive areas, the objects may not carry any electronic devices or power them off [7]. Thus, device-free indoor human behav- ior detection is in need, which detects and tracks the objects that do not carry any electronic devices nor participate actively in the process [8].

> 解决这些问题的一个常见方法是在Wi-Fi基础设施中确定移动设备的位置[5]。这样的解决方案需要移动设备的主动参与[6]。但是，在许多特殊场合，大多数人不会随时携带设备。例如在家或在敏感区域，物体可能不携带任何电子设备或关闭它们[7]。因此，需要设备免费的室内人类行为检测，检测和跟踪不携带任何电子设备，也不积极参与该过程的物体[8]。

In traditional indoor location technology, location service and environment perception are mainly based on received signal strength indication (RSSI) technology [9]. For example, researchers used RSSI to detect pre-positioning of indoor personnel in previous years [10]. 

> 在传统的室内位置技术中，位置服务和环境感知主要基于接收信号强度指示(RSSI)技术[9]。例如，研究人员在前几年使用RSSI来检测室内人员的预定位[10]。

However, in the experimental results of these technologies, it is found that the positioning accuracy is low. The main reason is that the stability of RSSI is low and the value is affected by time, and also, the time resolution is limited to the data packet standard [11]. 

> 然而，在这些技术的实验结果中，发现定位精度低。主要原因是RSSI的稳定性较低，值受到时间的影响，而且时间分辨率受到数据包标准的限制[11]。

In the IEEE 802.11n standard, the channel state information (CSI) describes the attenuation factor of each signal on each transmission path [12]. This is the value of each element in the channel gain matrix H, such as signal scattering, environmental weakness, and distance attenuation [13]. Compared with RSSI, CSI can adapt the communication system to the current channel conditions [14]. It has obvious advantages in terms of time resolution, frequency resolution, and stability [15]. Moreover, it is also guaranteed high-reliability and high-speed communication in the multi-antenna system [16].

> 在IEEE 802.11n标准中，信道状态信息(CSI)描述了每个信号在每个传输路径上的衰减因子[12]。这是信道增益矩阵H中每个元素的值，如信号散射、环境弱点和距离衰减[13]。与RSSI相比，CSI可以使通信系统适应当前的信道条件[14]。在时间分辨率、频率分辨率和稳定性方面都有明显的优势[15]。此外，它还保证了多天线系统中的高可靠性和高速通信[16]。

Wang et al. used a method of deterministic CSI fingerprinting and threshold-based [17]. In order to detect a human in an omnidirectional way, Wu et al. proposed DeMan [8] for device-free detection of moving and stationary human. 

> 王等人使用了一种确定性的CSI指纹识别和基于阈值的方法[17]。为了从全方位探测人类，吴等人提议使用DeMan[8]进行移动和静止的人类的设备免费检测。

DeMan took advantage of amplitude and phase information of CSI to detect moving human and considered human breathing as an intrinsic indicator of stationary human [18]. 

> DeMan利用CSI的振幅和相位信息来检测移动的人，并认为人的呼吸是静止人的固有指标[18]。

Lv et al. proposed speed independent device-free entity detection (SIED) which is suitable for intrusion detection of a different entity moving speeds [19]. SIED captured the variance of variances of amplitudes of each CSI subcarrier and combined. 

>  吕等人提出了一种适用于不同实体移动速度的入侵检测的速度独立的设备免费实体检测(SIED)[19]。SIED捕获了每个CSI子载波振幅的方差的方差并结合起来。

Zhu et al [16] proposed R-TTWD for detection of moving human by taking advantage of the correlated changes over different subcarriers. Furthermore, they extract the first-order difference of eigenvector of CSI amplitudes across different subcarriers. 

> 朱等人[16]提出了R-TTWD来通过利用不同子载波上的相关变化来检测移动的人。此外，他们提取了不同子载波上CSI振幅的特征向量的一阶差分。

Research [20] shows that the CSI can be used to achieve a detection rate of device-free human detection, and CSI is affected by human movements [21]. However, it is not optimal in timeliness of the algorithm. 

> 研究[20]表明，CSI可用于实现设备免费的人类检测的检测率，CSI受到人体运动的影响[21]。但是，在算法的时效性上不是最优的。

Research [13] shows that the CSI can be used to construct a low-cost and high-accuracy passive target localization method based on CSI model, which effectively combines the features of CSI with the target localization, which does not consider the detection area and detection rate relationship. 

> 研究[13]表明，CSI可用于构建基于CSI模型的低成本、高精度的被动目标定位方法，有效地结合了CSI的特征和目标定位，不考虑检测区域和检测率之间的关系。

Research [22] shows that the CSI is more temporally stable and provides the capability to benefit from the multipath effect, thus suitable for accurate human detection [23]. Research [24] seeks to monitor the position change of entities without actively carrying any physical devices. And research [25] shows that by creating a multiple hierarchical Dirichlet processes, NotiFi automatically learns the number of human body activity categories for abnormal detection.

>研究[22]表明，CSI在时间上更加稳定，并具有从多径效应中受益的能力，因此适合准确的人体检测[23]。研究[24]试图监控实体位置的变化，而不需要主动携带任何物理设备。研究[25]表明，通过创建多重层次的Dirichlet过程，NotiFi自动学习了人体活动类别的数量，用于异常检测。

Previous work on CSI-based human detection focused mainly on human behavior or used different methods to detect moving and stationary human separately [26]. The above papers all make good use of the characteristics of CSI, but some require special equipment to improve the positioning accuracy [27]. Furthermore, the overall performance and detection rate need to be improved to effectively achieve the accuracy of human behavior detection [28].

> 之前基于CSI的人类检测工作主要集中在人类行为上，或使用不同的方法分别检测移动和静止的人[26]。上述论文都很好地利用了CSI的特性，但有些需要特殊设备来提高定位精度[27]。此外，整体性能和检测率都需要进一步提高，以有效达到人类行为检测的准确性[28]。

In view of the problems mentioned above, in this paper, we present a method for indoor human behavior detection based on principal component analysis (PCA)-Kalman feature extraction. The process goes through CSI data collection, feature extraction, model training to establish the human detection classifier, and the relationship between CSI fingerprints. We propose to apply Kalman on the CSI data to reduce noise and the computing complexity. We will apply PCA to extract the most contributing eigenvalues from the original CSI data.

> 考虑到上述问题，在本文中，我们提出了一种基于主成分分析（PCA）-卡尔曼特征提取的室内人类行为检测方法。该过程包括CSI数据收集、特征提取、模型训练以建立人类检测分类器，以及CSI指纹之间的关系。我们建议在CSI数据上应用卡尔曼滤波来减少噪声和计算复杂性。我们将应用PCA从原始CSI数据中提取最具贡献的特征值。

At the same time, this method can reduce the dimension and reduce the time complexity of the algorithm. Meanwhile, we propose to apply support vector machine (SVM) on Wi-Fi CSI to achieve human detection. By doing this, the results of the classification will be converted into the physical location of the state information. In this paper, the Wi-Fi signal propagation model is used to detect the people’s different behavior in the physical space of human activity. Figure 1 shows the principle of the propagation of Wi-Fi signals in the physical space.

> 与此同时，这种方法可以降低维度并减少算法的时间复杂性。同时，我们建议在Wi-Fi CSI上应用支持向量机（SVM）来实现人类检测。这样做，分类的结果将被转换为状态信息的物理位置。在本文中，使用Wi-Fi信号传播模型来检测人们在人类活动的物理空间中的不同行为。图1显示了Wi-Fi信号在物理空间中传播的原理。

![在这里插入图片描述](https://img-blog.csdnimg.cn/7270384d75db4a4b8118a34771679c88.png#pic_center)

Whenever human beings live indoors, the radio signals that are accepted will change significantly, and the amplitude information of the CSI will also change sensitively [29]. Based on this principle, we propose a method based on ubiquitous Wi-Fi signals to detect personnel behavior [30]. It is feasible to use CSI data to get from COTS wireless device [31]. And no testers are required to carry any additional Wi-Fi infrastructure or special sensor [32]. It can work under the condition of line-of-sight (LOS), non-line-of-sight (NLOS), and through one wall.

> 每当人类生活在室内时，接收到的无线电信号都会发生明显变化，而CSI的幅度信息也会敏感地发生变化[29]。基于这一原理，我们提出了一种基于无处不在的Wi-Fi信号来检测人员行为的方法[30]。使用COTS无线设备获得的CSI数据是可行的[31]。而且测试人员不需要携带任何额外的Wi-Fi基础设施或特殊传感器[32]。它可以在视线（LOS）、非视线（NLOS）和穿过一堵墙的条件下工作。

Particularly, this method provides a detection rate of around 92% for both moving and stationary people, while identifies human-free scenarios by 95%; all of which outperforms existing methods by about 20%. This method is greatly improved in the detection accuracy and overall efficiency compared with the traditional indoor personnel detection method.

> 特别地，这种方法对于移动和静止的人都提供了大约92%的检测率，同时通过95%来识别没有人的场景；所有这些都比现有的方法高出大约20%。与传统的室内人员检测方法相比，此方法在检测精度和整体效率上都得到了很大的提高。

In summary, the main contributions of the paper are as follows:

> 总之，本文的主要贡献如下：

a) We propose to use the fine-grained PHY layer information CSI for human behavior detection and, to the best of our knowledge, present a method for indoor human behavior detection based on princi pal component analysis (PCA)-Kalman feature extraction.

> a) 我们建议使用精细的物理层信息CSI进行人类行为检测，并且据我们所知，提出了一种基于主成分分析（PCA）-卡尔曼特征提取的室内人类行为检测方法。

b) We adopt Kalman algorithm to reduce noise in CSI data and apply PCA to extract the most contributing features from the CSI and reduce the dimensionality.

> b) 我们采用卡尔曼算法来减少CSI数据中的噪声，并应用PCA从CSI中提取最主要的特征并减少维度。

c) Apply SVM classification to perform device-free human detection regardless of human moving or stationary.

> c) 应用SVM分类来进行设备自由的人类检测，无论人类是移动还是静止。

d) Regarding the performance of human detection, compare PCA-Kalman with other indoor human behavior detection methods. Investigate the parameters that affect the performance of human detection, such as the number of access points and the number of the packet and window size. Experiment results demonstrate that the PCA-Kalman can achieve high performance that outperforms traditional CSI-based system.

> d) 关于人类检测的性能，将PCA-Kalman与其他室内人类行为检测方法进行比较。研究影响人类检测性能的参数，例如接入点的数量、数据包的数量和窗口大小。实验结果表明，PCA-Kalman可以实现高性能，优于传统的基于CSI的系统。

The rest of the paper is organized as follows. We review the related works in Section 2 briefly. In Section 3, we present the method design and the design details of with PCA-Kalman. We present the experimental setup with PCA-Kalman in Section 4. Evaluations, comparisons, results and discussion are reported in Section 5. Finally, we conclude the work in Section 6.

> 本文的其余部分组织如下。在第2部分，我们简要回顾了相关工作。在第3部分，我们介绍了方法设计以及PCA-Kalman的设计细节。在第4部分，我们介绍了与PCA-Kalman的实验设置。第5部分报告了评估、比较、结果和讨论。最后，在第6部分，我们总结了这项工作。

<a id="2_related_works"></a>
# 2 Related works

<a id="2_1_human_behavior"></a>
## 2.1 Human behavior detection with CSI

With the rise of cloud computing and the Internet of Things, as an enabling technology, network physical systems (CPS) are almost everywhere today [33, 34]. Yang et al. [35] proposed a complete data replica manager solution called “Auto Replica,” working in distributed caching and data processing systems using SSD-HDD tier storages. Auto Replica balances the trade-off between the performance and fault tolerance by storing caches in replica nodes’ SSDs. 

> 随着云计算和物联网的兴起，作为一种启用技术，网络物理系统（CPS）今天几乎无处不在[33, 34]。杨等人[35]提出了一个完整的数据副本管理解决方案，称为“Auto Replica”，该解决方案在使用SSD-HDD层存储的分布式缓存和数据处理系统中工作。Auto Replica通过在副本节点的SSD中存储缓存来平衡性能和容错性之间的权衡。

At the same time, we are in the era of data explosion and cloud computing [36], and large-scale data analysis is of great significance in various fields of research and industry [37]. With these technical supports, indoor positioning technology and indoor personnel status detection technology will be favored by all fields of research, and indoor positioning and indoor personnel status testing will become an essential part of life. 

> 与此同时，我们正处于数据爆炸和云计算的时代[36]，大规模数据分析在研究和工业的各个领域都具有重要意义[37]。有了这些技术支持，室内定位技术和室内人员状态检测技术将受到所有研究领域的青睐，室内定位和室内人员状态测试将成为生活的必要部分。

Hu et al [38] proposed a method to detect human motion based on phase eigenvalue and then combined with covariance matrix and dynamic time window algorithm; finally, the experimental results of the HPMD system obtained a high-detection rate. Zhou et al [39] put forward a passive indoor location and detection method based on CSI. This method firstly collects effective CSI data, uses PCA algorithm for feature extraction and dimensionality reduction, then establishes fingerprint database, and finally, carries out online detection. The method is tested in two typical environments of visual distance and non-visual distance, and the detection rate is as high as 97% in the end. 

> 胡等人[38]提出了一种基于相位特征值检测人体运动的方法，然后结合协方差矩阵和动态时间窗口算法；最终，HPMD系统的实验结果获得了高检测率。周等人[39]提出了一种基于CSI的被动室内定位和检测方法。此方法首先收集有效的CSI数据，使用PCA算法进行特征提取和降维，然后建立指纹数据库，最后进行在线检测。该方法在视觉距离和非视觉距离的两种典型环境中进行了测试，最终检测率高达97%。

Compared with the traditional method of human behavior detection, this method improves the detection rate to a great extent. Wang et al [40] used CSI to detect the basic motion of human body, and CSI can be applied in real life. It can be seen that the use of CSI for indoor personnel status detection has developed very quickly, which will be the future development trend of wireless sensor network.

>与传统的人类行为检测方法相比，此方法在很大程度上提高了检测率。王等人[40]使用CSI检测人体的基本动作，CSI可以在现实生活中应用。可以看出，使用CSI进行室内人员状态检测发展得非常快，这将是无线传感器网络的未来发展趋势。

<a id="2_2_novel_applications"></a>
## 2.2 Novel applications based on CSI

Zhu et al [41] used CSI to analyze a series of effects of human motion on CSI, so a more robust passive human motion detection method was proposed, which uses PCA algorithm and spatial diversity provided by multiple antennas. Through the study of the quality of each antenna, the best antenna combination is selected to improve the accuracy of detection. 

> 朱等人[41]使用CSI来分析人体运动对CSI的一系列影响，因此提出了一种更稳健的被动人体运动检测方法，该方法使用PCA算法和由多个天线提供的空间多样性。通过研究每个天线的质量，选择最佳天线组合来提高检测的准确性。

Yan Wang et al [42] presented device-free location-oriented activity identification at home through the use of CSI. Domenico et al [43] presented a device-free crowd counting and occupancy estimation system by analyzing the shape of the Doppler spectrum of the received signal which was correlated to the number of people moving in the monitored environment. It can be seen that the detection of indoor human behavior detection using CSI signals has become a trend for the development of wireless sensor networks in the future.

> Yan Wang等人[42]通过使用CSI，在家中提出了无设备的位置导向活动识别。Domenico等人[43]通过分析与在被监测环境中移动的人数相关的接收信号的多普勒谱的形状，提出了一个无设备的人群计数和占用估计系统。可以看出，使用CSI信号进行室内人类行为检测已成为未来无线传感器网络发展的趋势。

<a id="2_3_csi_data">
## 2.3 CSI data collection

With the popularity of wireless networks and advances in orthogonal frequency division multiplexing (OFDM) technology, the traditional landscape has changed [31]. Channel responses can be extracted from commercial Wi-Fi devices under 802.11/g/n standards, which reveals that a set of channel measurements depict the amplitude and phase of each subcarrier [44]. Because RSSI cannot distinguish multiple signal propagation paths one by one [45], different multipath propagation conditions may lead to different sensitivities based on human movement, which further affects the results of human detection [46].

> 随着无线网络的普及和正交频率分割复用技术（OFDM）的进步，传统的格局已经发生变化[31]。根据802.11/g/n标准，可以从商用Wi-Fi设备中提取信道响应，这表明一组信道测量描述了每个子载波的幅度和相位[44]。因为RSSI无法一一区分多个信号传播路径[45]，所以不同的多径传播条件可能会根据人的移动导致不同的灵敏度，进一步影响人类检测的结果[46]。

In multi-path indoor environment, wireless signals can change signal propagation in a more complicated way through reflection, diffraction and scattering, and human motion, which results in different amplitude signals under different sensitivities [47]. We were tested under three scenes line-of-sight dis- tance, non-line-of-sight distance, and wall.

> 在多路径室内环境中，无线信号可以通过反射、衍射、散射和人体运动以更复杂的方式改变信号传播，从而导致不同的灵敏度下产生不同的幅度信号[47]。我们在三种情景下进行了测试：视线距离、非视线距离和墙壁。

In order to evaluate PCA-Kalman’s ability to achieve LOS, NLOS, and through one wall behavior recognition. We extensively valuate PCA-Kalman’s performance in the following three scenarios, as shown in Fig. 2.

> 为了评估PCA-Kalman在视距（LOS）、非视距（NLOS）和穿墙行为识别方面的能力，我们广泛地评估了PCA-Kalman在以下三种情景下的表现，如图2所示。

a) LOS. The tester is in the LOS range between the transmitter and the receiver.

> a) 视距（LOS）。测试者位于发射机和接收机之间的视距范围内。

b) NLOS. The tester is not on the LOS between the transmitter and the receiver, but within their range. 

> b) 非视距（NLOS）。测试者不在发射机和接收机之间的视距内，但在它们的范围内。

c) Through the wall. The transmitter and the receiver are in adjacent rooms.

> c) 穿过墙壁。发射机和接收机位于相邻的房间内。

![在这里插入图片描述](https://img-blog.csdnimg.cn/3de29ae4256645b2a27307d014c4efe0.png#pic_center)

As shown in Fig. 2, it can be seen that the distance between the signal sending end and the signal receiving terminal is different, and the different position of the personnel and the different actions of the body make an influence on the change of the signal. Therefore, the number of non-visual paths may interfere with human motion. More paths lead to more severe fluctuations in the received signals. 

> 如图2所示，可以看出信号发送端与信号接收端之间的距离不同，人员的不同位置和身体的不同动作都会影响信号的变化。因此，非视觉路径的数量可能会干扰人体的运动。更多的路径导致接收到的信号中的波动更为严重。

In contrast, the LOS path, the person’s movement has no significant influence on the received signal. Therefore, from Fig. 3, we can see that in different environments, the distance between receiver and transmitter, location, and other factors have great influence on the sensitivity of received signals. It can be observed from Fig. 3a, b that there are great differences in amplitude signals in different environments. Figure 3a is more stable than Fig. 3b because Fig. 3a is collected in an open hall environment and has less interference. Figure 3b is a collection of data in a relatively complex laboratory environment, where there are a lot of electromagnetic interference, such as mobile phones and computers. And there are a lot of multipath interference, such as desks and chairs. This results in a large fluctuation of the signal in Fig. 3b. Therefore, through the comparison of two sets of amplitudes of CSI in different environments in Fig. 3, it can be concluded that the amplitude signals of CSI are different under different conditions. This phenomenon proves that it is feasible to use the amplitude signal of CSI as the characteristic value of indoor personnel state detection.

> 相比之下，在视距（LOS）路径下，人的移动对接收到的信号没有明显的影响。因此，从图3中，我们可以看出在不同的环境下，接收器和发射器之间的距离、位置和其他因素对接收信号的灵敏度有很大的影响。从图3a和3b中可以观察到，在不同的环境中，幅度信号存在很大的差异。图3a比图3b更稳定，因为图3a是在一个开放的大厅环境中收集的，干扰较少。图3b是在一个相对复杂的实验室环境中收集的数据，其中存在很多电磁干扰，例如手机和电脑。还有很多多径干扰，例如桌子和椅子。这导致图3b中信号的大幅波动。因此，通过对比图3中不同环境下的CSI的两组幅度，可以得出结论：在不同条件下，CSI的幅度信号是不同的。这一现象证明，使用CSI的幅度信号作为室内人员状态检测的特征值是可行的。

![在这里插入图片描述](https://img-blog.csdnimg.cn/94a97e09c353486cbc6688c64a4b29b5.png#pic_center)

In order to better represent the multipath propagation, in the modeling stage, we use channel impulse response (CIR) to build the model. In the case of the linear time constant, CIR can be expressed as:

> 为了更好地表示多径传播，在建模阶段，我们使用信道冲激响应（CIR）来构建模型。在线性时常情况下，CIR可以表示为：

$$
h(r) = \sum_{i=1}^N a_i e^{-j \theta_i \delta (\tau - \tau_i)}
$$

Phase offset and time delay wherever $a_i$、$\theta_i$, and $\tau_i$ are respectively the ith path attenuating. N represents the total number of propagation paths, and $\delta(\tau)$ is a Dirac pulse function. Due to frequency selective fading during multipath propagation, in order to better characterize multipath propagation, we can select a channel frequency response (CFR) in order to better characterize multipath propagation. Since CFR is composed of amplitude-frequency response and phase-frequency response, CSI is fine-grained information from the physical layer that describes CFR from the transmitter to the receiver. Leveraging commodity network interface card (NIC) with modified firmware and driver, the amplitude and phase of each subcarrier within a channel can be revealed to the upper layers for each packet in the format of CSI.

> 相位偏移和时间延迟，其中 $a_i$、$\theta_i$ 和 $\tau_i$ 分别是第 i 条路径的衰减。N 代表传播路径的总数，而 $\delta(\tau)$ 是 Dirac 脉冲函数。由于多径传播过程中的频率选择性衰落，为了更好地表征多径传播，我们可以选择一个信道频率响应（CFR）来更好地表征多径传播。由于 CFR 是由幅度频率响应和相位频率响应组成的，CSI 是从发射机到接收机描述 CFR 的物理层的细粒度信息。利用具有修改过的固件和驱动程序的商品网络接口卡（NIC），可以将每个子载波的幅度和相位在每个数据包的 CSI 格式中显示给上层。

Each CSI group represents the amplitude and phase of an OFDM subcarrier, which can be expressed as:

> 每个CSI组代表一个OFDM子载波的幅度和相位，可以表示为：

$$
H(k) = \|  H(k) \| e^{j∠H(k)}
$$

In the formula above,$\|  H(k) \| e^{j∠H(k)}$ are amplitude and phase of the k-th subcarrier, respectively, since the amplitude information is more stable. Moreover, we use the amplitude to represent eigenvalue in the following process.

> 在上面的公式中，$\|  H(k) \| e^{j∠H(k)}$ 分别是第k个子载波的幅度和相位，由于幅度信息更为稳定。此外，我们使用振幅去表示之后的特征值。

<a id="2_3_csi_feature"></a>
## 2.4 CSI feature extraction

In this paper, we collected CSI data in three different scenarios, and the data were obtained from common commercial Wi-Fi equipment, which is the real data. Because of the inherent noise of the environment and a series of electromagnetic interference, this will make the data unstable. In this paper, Kalman filter algorithm is used to reduce noise, and then PCA algorithm is used to extract the feature of the filtered eigenvalue. The combination of PCA algorithm and Kalman filter algorithm is the innovation of this paper. It not only make use of the better timeliness of Kalman filter algorithm, but also make use of the dimension reduction function of PCA algorithm.

> 在本文中，我们在三种不同的场景下收集了CSI数据，数据来自常见的商用Wi-Fi设备，这是真实的数据。由于环境的固有噪声和一系列电磁干扰，这将使数据不稳定。在本文中，使用了卡尔曼滤波算法来降低噪声，然后使用PCA算法提取过滤后的特征值。PCA算法和卡尔曼滤波算法的结合是本文的创新之处。它不仅利用了卡尔曼滤波算法的良好时效性，而且利用了PCA算法的降维功能。

Through repeated experiments, the experimental results show that the method proposed in this paper has good robustness and is tested in three different environments, so the method is more practical. We apply the Kalman algorithm to detect and remove the noise. In the first place, the goal of indoor exer- cise should be modeled. Figure 4a shows the raw CSI data collected from three receiving antennas at the receiving end, in which the isolated little black curve represents the noise and is marked with blue dashed lines. Figure 4b shows the CSI signal after removing redundant noise using the Kalman filter algorithm.

> 通过反复的实验，实验结果表明本文提出的方法具有良好的鲁棒性，并在三种不同的环境中进行了测试，因此该方法更具实用性。我们应用卡尔曼算法来检测并去除噪声。首先，室内运动的目标应该是建模。图4a显示了从接收端的三个接收天线收集的原始CSI数据，其中孤立的小黑曲线代表噪声，并用蓝色虚线标出。图4b显示了使用卡尔曼滤波算法去除多余噪声后的CSI信号。

![在这里插入图片描述](https://img-blog.csdnimg.cn/ab7cc69af3e64a8aa56def9418ead3b3.png#pic_center)

In our laboratory environment, we collected 360 dimensional CSI samples. But in the open hall environment, the CSI sample contains 270 dimensions. At the same time, high dimensionality causes time complexity. As each dimension may have a different contribution to human behavior detection, we apply PCA algorithm to extract the most contributing features and reduce the dimensionality of CSI data. PCA algorithm is based on the maximum variance of the high-dimensional feature space data, and then projected into the low-dimensional space through linear transformation, so that more indexes can be converted into several unique comprehensive indicators. The core of PCA is to diagonalize the covariance matrix so as to minimize the correlation between dimensions. Furthermore, it can raise and preserve the energy of the dimension. Specifically, as a high-dimensional data set, the raw data is maintained to the maximum extent while reducing the di- mension and eliminating redundant data and noise as much as possible.

> 在我们的实验室环境中，我们收集了360维的CSI样本。但在开放的大厅环境中，CSI样本包含270个维度。同时，高维度导致时间复杂性增加。由于每个维度可能对人的行为检测有不同的贡献，我们应用PCA算法提取最有贡献的特征，并降低CSI数据的维度。PCA算法基于高维特征空间数据的最大方差，然后通过线性变换投影到低维空间，从而更多的指标可以转化为几个独特的综合指标。PCA的核心是对协方差矩阵进行对角化，从而最大化降低维度之间的相关性。此外，它可以提高并保留维度的能量。具体地说，作为一个高维数据集，原始数据在降维的同时最大程度地保持，同时尽可能地消除冗余数据和噪声。

If the CSI signal is directly used as a fingerprint fea- ture, the dimension of data is high, and the parameter estimation is difficult and computationally expensive. Furthermore, we use PCA to extract the features and remove the correlation between the location features and the components. This method reduces not only the dimension and the amount of data, but also the noise in the CSI signal and the positioning error. Consequently, we extract the eigenvalues directly from the subcarriers; thus, we extract more useful eigenvalues. We deployed N reference points in the selected positioning area. 

> 如果直接使用CSI信号作为指纹特征，数据的维度很高，参数估计困难且计算成本高。此外，我们使用PCA来提取特征并消除位置特征和组件之间的相关性。这种方法不仅减少了维度和数据量，而且减少了CSI信号中的噪声和定位误差。因此，我们直接从子载波中提取特征值；从而提取更多有用的特征值。我们在选定的定位区域部署了N个参考点。

The position $p_i(x_i, y_i)$ of each reference point and the physical position of N reference points constitute a position space $P = (p_1, p_2, ...p_N)^T$. The CSI signals of the position points are collected at each reference point, and each reference point is collected $n$ times. And the collected signal is taken as the origin of the $p_i(x_i, y_i)$. Fingerprint information is recorded as an n-dimensional vector, $Q_i = (csi_1, csi_2, ..., csi_n)^T$,$i \in (1, N)$, where each CSI signal is again a complex matrix: $m \times n \times 56$.

> 每个参考点的位置$p_i(x_i, y_i)$和N个参考点的物理位置构成一个位置空间$P = (p_1, p_2, ...p_N)^T$。在每个参考点处收集位置点的CSI信号，并且每个参考点收集$n$次。并且，收集到的信号被视为$p_i(x_i, y_i)$的起点。指纹信息被记录为一个n维向量，$Q_i = (csi_1, csi_2, ..., csi_n)^T$，$i \in (1, N)$，其中每个CSI信号再次是一个复数矩阵：$m \times n \times 56$。

We use the original position fingerprints of all reference points to make up an original position fingerprint space Q of $n \times N$ dimension, where Q is the $Q = (Q_1, Q_2, ..., Q_N)^T$, which Q is expressed as each row vector in the matrix. Q represents the original position fingerprints of the reference points.

> 我们使用所有参考点的原始位置指纹来组成一个$n \times N$维的原始位置指纹空间Q，其中Q是$Q = (Q_1, Q_2, ..., Q_N)^T$，Q表示为矩阵中的每一行向量。Q代表参考点的原始位置指纹。

<a id="3_pca_kalman"></a>
# 3. PCA-Kalman design

This section presents the design of PCA-Kalman in a top-down manner, with emphasis on indoor human behavior detection. The architecture of PCA-Kalman mainly consists of two components: offline training phase and online testing phase. The working principle of using PCA-Kalman method to detect the behavior state of indoor personnel is shown in Fig. 5.

![在这里插入图片描述](https://img-blog.csdnimg.cn/ee9186f1822041f1ac5e4422b6c6d4fb.png#pic_center)

> 本节从自上而下的方式介绍PCA-Kalman的设计，重点在室内人类行为检测上。PCA-Kalman的架构主要由两个部分组成：离线训练阶段和在线测试阶段。使用PCA-Kalman方法检测室内人员的行为状态的工作原理如图5所示。

<a id="3_1_offline"></a>
## 3.1 Offline training phase

In this section, we collect the position coordinates of each test area and process the received raw data of CSI. In this process, we leverage Kalman filter algorithm to reduce the noise of the original data and use the improved PCA algorithm to extract the most contributing features and reduce the dimensionality of CSI fingerprints. In other words, we extract the nonlinear feature of the original position fingerprint. Furthermore, we store the processed CSI signal in the fingerprint database and update the fingerprint data- base in real time according to the environment change.

> 在这个部分，我们收集每个测试区域的位置坐标并处理收到的CSI的原始数据。在这个过程中，我们利用Kalman滤波算法来减少原始数据的噪音，并使用改进的PCA算法提取最有贡献的特征并减少CSI指纹的维度。换句话说，我们提取了原始位置指纹的非线性特征。此外，我们将处理过的CSI信号存储在指纹数据库中，并根据环境变化实时更新指纹数据库。

In the first place, we derive a frequency-domain model of the state of a single channel. The formula can be expressed as:

> 首先，我们推导出单信道状态的频域模型。该公式可表示为

$$
Y = HX + N
$$

where Y is the received signal vector, X is a vector of the transmitted signal, H is the channel matrix, and N represents additional Gaussian white noise vectors. According to the previous step, CSI of all subcarriers can be expressed as:

> 其中，Y 是接收信号矢量，X 是发射信号矢量，H 是信道矩阵，N 代表附加高斯白噪声矢量。根据上一步，所有子载波的 CSI 可表示为

$$
CSI = \frac{Y}{X}
$$

Then, the CSI of a single subcarrier can be expressed as:

> 然后，单个子载波可以 表示为：

$$
csi = | csi | e^{ j sin ∠ csi}
$$

where $|csi|$ and $∠csi$ are the amplitude and phase corresponding to the subcarrier, respectively. Furthermore, we propose to apply Kalman filter algorithm to filter the amplitude information in the previous step. Suppose Xk to represent the state vector of the system at time k. If the change in the state quantity of the system can be expressed in the form of a linear equation. Then, the equation which becomes the state transition equation of the system can be expressed as:

> 其中，$|csi|$ 和 $∠csi$ 分别是子载波的幅度和相位。此外，我们建议在前一步应用Kalman滤波算法来过滤幅度信息。假设 Xk 代表系统在时间k的状态向量。如果系统的状态量变化可以用线性方程的形式表示。那么，这个方程成为系统的状态转移方程可以表示为：

$$
X_k = F_k X_{k-1} + B_k U_k + W_k
$$

where $F_k$ is the state transition matrix, $B_k$ is the control matrix, $U_k$ is the control vector, and $W_k$ is the noise of the transfer process. After Kalman filtering, these redundant noises are removed effectively.

> 其中，$F_k$ 是状态转移矩阵，$B_k$ 是控制矩阵，$U_k$ 是控制向量，而 $W_k$ 是转移过程的噪声。经过Kalman滤波后，这些冗余噪声被有效地去除。

We use the PCA algorithm to extract the main features of the original fingerprint space, and remove the redundant data to form the feature location fingerprint space, which can be expressed by the formula:

> 我们使用PCA算法来提取原始指纹空间的主要特征，并去除冗余数据形成特征位置指纹空间，该空间可以通过以下公式来表示：

$$
Q' = (Q_1', Q_2', \cdots, Q_N')^T
$$

The specific algorithm is as follows:

> 该特殊算法如下：

Step 1: Based on the above sample space formula, we can derive a new formula:

> 步骤1: 根据以上样本空间公式，我们可以得到一个新的公式

$$
Q = (Q_1, Q_2, \cdots, Q_N)^T
$$

Then, we calculate the center of the feature again. We use the data of each dimension minus the mean of the dimension data, in order to ensure that the average value of each dimension is zero. So, M rep- resents the matrix.

> 然后，我们再次计算特征的中心。我们使用每个维度的数据减去维度数据的平均值，以确保每个维度的平均值为零。因此，M 代表矩阵。

Step 2: Construct the covariance matrix C:

> 步骤2:构建协方差矩阵C

$$
C = \frac{1}{N} MM^T
$$

Step 3: Calculate the eigenvalue $\lambda_i$ and eigenvector $v_i$ of the covariance matrix and then select the largest m eigenvectors:

> 步骤 3：计算协方差矩阵的特征值 $\lambda_i$ 和特征向量 $v_i$，然后选出最大的 m 个特征向量：

Step 4: We project the original sample matrix:

> 步骤 4：投影原始样本矩阵：

Finally, we can get matrix $Q'$ after dimension reduction.

> 最后，我们可以得到降维后的矩阵 $Q'$。

<a id="3_2_online"></a>
## 3.2 Online behavior testing phase

![在这里插入图片描述](https://img-blog.csdnimg.cn/b21a74e7ce3d4b1ba78368428d56b5a6.png#pic_center)

![在这里插入图片描述](https://img-blog.csdnimg.cn/5ff13864476a4fc3b5c68a7a26d8e706.png#pic_center)

![在这里插入图片描述](https://img-blog.csdnimg.cn/9f97076fa1a14ee4b68f0b0dd3687f98.png#pic_center)


At this stage, the transmitter is responsible for collecting real-time CSI data of each testing area. Similarly, the sender sends data on different behaviors made by the tester to the receiving end. Then, the SVM classifier is applied to the physical position coordinate state, and the classification result is obtained. Finally, the state of the estimated position is obtained. The main steps are as follows:

> 在这个阶段，发射器负责收集每个测试区的实时CSI数据。同样，发送器将测试人员做出的不同行为的数据发送到接收端。然后，将SVM分类器应用于物理位置坐标状态，并获得分类结果。最后，获得估计位置的状态。主要步骤如下：

In the initialization phase, we divided the test area into 20 squares of the same size. Furthermore, we use these squares as reference points. The volunteers traverse the reference points one by one and perform different action states at each reference point, such as standing, jumping, squatting, sitting down, and other common actions.

> 在初始化阶段，我们将测试区域划分为20个大小相同的正方形。此外，我们使用这些正方形作为参考点。志愿者逐一遍历参考点，在每个参考点进行不同的动作状态，如站立、跳跃、蹲下、坐下和其他常见动作。

The obtained CSI data is transferred to the server. As can be observed from Fig. 6, the changes in CSI signals vary significantly when testers perform different operations. What is more, real-time test data are collected in real experiment environment.

> 获得的CSI数据被传输到服务器。从图6中可以观察到，当测试者执行不同的操作时，CSI信号的变化显著不同。更重要的是，实时测试数据是在真实的实验环境中收集的。

![在这里插入图片描述](https://img-blog.csdnimg.cn/4564decf0d444e33b4ff7a1ad49fd755.png#pic_center)
![在这里插入图片描述](https://img-blog.csdnimg.cn/29825fa98a3d464597a623928ed30f91.png#pic_center)

The next step is to apply the SVM classifier as follows:

> 下一步是应用SVM分类器，具体步骤如下：

Step 1: We assume that the sampling period for the CSI data at the receiving end is $T$, which is usually 1.0 s. Firstly, we let a volunteer traverse all reference points and perform daily actions such as standing, jumping, squatting, and sitting at the refer- ence point. Assuming that the receiving end samples the sample training set is $\{ (x_i, y_i) \}$, $i=1, 2, \cdots, n$. 

> 步骤1：我们假设接收端的CSI数据的采样周期为$T$，通常是1.0秒。首先，我们让一名志愿者遍历所有的参考点，并在参考点上进行日常动作，如站立、跳跃、蹲下和坐下。假设接收端采样的样本训练集是${ (x_i, y_i) }$，$i=1, 2, \cdots, n$。

The SVM regression function is

> SVM回归函数是

(10)
$$
f (X) = w \cdot x + b
$$

In the above formula, $W$ is the weight vector, and $b$ is the bias vector.

> 在上面的函数，$W$是权重向量，$b$是偏差向量。

Step 2: By solving the convex quadratic programming problem to solve $w$ and $b$ in Eq. (17), the predictive function is obtained, and the objective function is minimized:

> 步骤2:  通过求解凸二次规划问题求解式(17)中的 $w$ 和 $b$，得到预测函数，最小化目标函数

(11)
$$
Q(w) = \frac{1}{2} \|  w \|^2 + C \cdot R_{emp}(f)
$$

where C is the penalty factor, $R_{emp}(f)$ is the loss function.

> 其中C是惩罚因子，$R_{emp}(f)$是损失函数。

![在这里插入图片描述](https://img-blog.csdnimg.cn/8d71c48c76ed4541bcab0df0a54ec535.png#pic_center)


Step 3: Using the relaxation factor to convert the optimization objective function of formula (11) into:

> 使用松弛因子将公式（11）的优化目标函数转化为：

(12)
$$
\min \frac{1}{2} \| w \|^2 + C \sum_{i-1}^{l} (\xi_i + \xi_i^*)
$$

where $\xi_i$; $\xi_i^*$ are the relaxation factors.

> 其中，$\xi_i$；$\xi_i^*$ 是松弛因子。

![在这里插入图片描述](https://img-blog.csdnimg.cn/5954910d64674c42b1bf0fa848b5145d.png#pic_center)


Step 4: To speed up the solution, we choose to introduce the Lagrange function and solve the following dual form to obtain multipliers αi;αi , expressed as:

> 步骤4：为了加速解决方案，我们选择引入拉格朗日函数，并求解以下的对偶形式来获取乘子 αi；αi，表示为：

(13)
$$
\max = \sum_{i=1}^l (a_i - a_i^*) y_i - \varepsilon \sum_{i=1}^{l} (a_i + a_i^*) - \frac{1}{2} \sum_{i,j=1}^{l} (a_i^* - a_i)(a_j^* + a_j) <x_i, x_j>
$$

Step 5: For nonlinear predictive problems, SVM introduces the mapping function $\phi$ to map the original data into a new feature space. Then, we convert the nonlinear problem into a linear problem in the new feature space. At the same time, the SVM regression can be expressed as:

> 步骤5：对于非线性预测问题，SVM引入映射函数$\phi$将原始数据映射到一个新的特征空间。然后，我们将新特征空间中的非线性问题转化为线性问题。与此同时，SVM回归可以表示为：

(14)
$$
f(x) = w \cdot \phi(x) + b
$$

Step 6: The optimization objective function of formula (13) is transformed into the following form:

> 步骤6：将公式（13）的优化目标函数转化为以下形式：

![在这里插入图片描述](https://img-blog.csdnimg.cn/b60c948499524d2d86e32ae6c1e31085.png#pic_center)

![在这里插入图片描述](https://img-blog.csdnimg.cn/79b32de013d14d5097ab346c9c0af1fb.png#pic_center)

(15)
$$
\max = \sum_{i-1}^l (a_i - a_i^*) y_i - \xi \sum_{i=1}^l (a_i + a_i^*) - \frac{1}{2} \sum_{i,j=1}^l (a_i^* - a_i) (a_j^* - a_j) <\phi (x_i), \phi(x_j)>
$$

Step 7: In this step, we introduce kernel function $k(x_i, x_j)$ instead of $(\phi (x_i), \phi (x_j))$ formula. Additionally, (15) is transformed into the following form of optimization objective:

> 在此步骤中，我们引入核函数 $k(x_i, x_j)$ 替代 $(\phi (x_i), \phi (x_j))$。另外，(15) 转化为以下优化目标的形式：

(16)
$$
\max = \sum_{i-1}^{l} (a_i - a_i^*) y_i - \xi \sum_{i=1}^{l} (a_i + a_i^*) - \frac{1}{2} \sum_{i,j=1}^l (a_i^* - a_i)(a_j^* - a_j) k(x_i, x_j)
$$

Step 8: What is more, the final SVM prediction function is:
> 步骤8：更进一步，最终的SVM预测函数为：

(17)
$$
f(x, a_i, a_j^*) = \sum_{i=1}^l (a_i - a_i^*) k(x_i, x_j) + b
$$

Step 9: We use real-time amplitude data collected and fingerprint data to match. When the testers conduct experiments on different actions in the test area, the amplitude characteristics change with the change of different conditions. It can be clearly seen from Fig. 6 that the amplitude changes corresponding to different actions are completely different.

> 步骤9：我们使用收集的实时幅度数据与指纹数据进行匹配。当测试者在测试区域对不同的动作进行实验时，幅度特性随着不同条件的变化而变化。从图6中可以清楚地看到，与不同动作相对应的幅度变化是完全不同的。

<a id="4_experimental"></a>
# 4 Experimental setup

<a id="4_1_hardware"></a>
## 4.1 Hardware testbed

Both the transmitter and the receiver have installed the Atheros AR 9380 NIC supporting the IEEE802.11n protocol to facilitate the acquisition of CSI from the device. As shown in Fig. 9. We have in- stalled an external antenna of about 1.5 m in length. And all transmitters are equipped with the Ubuntu 14.04 LTS 32-bit system and the 4.1.10 Linux kernel version. At the same time, we use open source drivers, Atheros-CSI-Tool, and Atheros NIC, developed by Hsieh and others.

> 发送器和接收器均安装了支持IEEE802.11n协议的Atheros AR 9380 NIC，以便从设备中获取CSI。如图9所示。我们安装了一个约1.5米长的外部天线。并且所有发射器都配备了Ubuntu 14.04 LTS 32位系统和4.1.10 Linux内核版本。同时，我们使用由Hsieh等人开发的开源驱动程序，Atheros-CSI-Tool和Atheros NIC。

The driver modules make it possible to obtain CSI from Linux kernel when it received signals and saved them for other process. One of the desktop computers equipped with Intel Core i3-4150 CPU works as a sender while another computer works as a receiver. In our system, the obtained data is processed using the PCA-Kalman algorithm in C program.

> 驱动模块使得在接收信号时从Linux内核获取CSI并保存它们以供其他进程使用成为可能。其中一台配备Intel Core i3-4150 CPU的桌面计算机作为发送器，而另一台计算机作为接收器。在我们的系统中，使用PCA-Kalman算法在C程序中处理获得的数据。

![在这里插入图片描述](https://img-blog.csdnimg.cn/31295772c4214ed9a5cddc7b8e2bedb6.png#pic_center)

<a id="4_2_experimental"></a>
## 4.2 Experimental scenarios

We conducted our experiments in a research laboratory (8m×9m) and a meeting room (6m×4m) both piled with desks and computers as well as a hall (10 m ×9 m) in our campus, which is piled with desks and computers in the laboratory and meeting room, thus creating a rather complex multipath environment, as shown in Figs. 7 and 8. We evaluated the performance of PCA-Kalman in three rooms’ surroundings (Tables 1 and 2).

> 我们在一个研究实验室（8m×9m）、一个会议室（6m×4m）以及我们校园内的一个大厅（10m×9m）进行了实验。实验室和会议室里堆放着桌子和计算机，大厅里也堆放了桌子和计算机，从而在图7和8中展示了一个相当复杂的多路径环境。我们评估了PCA-Kalman在三个房间环境中的性能（表1和表2）。

![在这里插入图片描述](https://img-blog.csdnimg.cn/aad3434a01ad44c087f8787e1a9c9b68.png#pic_center)


In order to construct an offline histogram fingerprint, we gathered data first thing in the morning when no one was in the lab. In addition, for the purpose of behavior detection, we generated three test sets, including three different test areas. The infrastructure of indoor human behavior detection is composed of wireless access points (AP) for data transmission, monitoring points (MP) for data retrieval, and a server for data processing. Each AP continuously broadcasts information to facilitate the Wi-Fi device to find an access point. For complete coverage of the area of interest, each pair of AP-MP is placed at the corners or edges, forming diagonal lines [48]. Each pair of transmitting-receiving anten- nas is a link (Fig. 9).

> 为了构建一个离线直方图指纹，我们在早上最早的时候收集了数据，当时实验室里没有人。另外，为了行为检测的目的，我们生成了三个测试集，包括三个不同的测试区域。室内人体行为检测的基础设施由无线接入点（AP）组成，用于数据传输，监测点（MP）用于数据检索，以及一个用于数据处理的服务器。每个AP不断地广播信息，以帮助Wi-Fi设备找到一个接入点。为了完全覆盖感兴趣的区域，每对AP-MP都放置在角落或边缘，形成对角线[48]。每对发送-接收天线是一个链接（图9）。

<a id="5_numerical"></a>
# 5 Numerical results and discussion

In this section, we interpret the experimental setups and the analysis of system performance and parameters.

> 在本节中，我们将解释实验设置以及对性能和参数的分析。

![在这里插入图片描述](https://img-blog.csdnimg.cn/0e4b2c408de04440aad1c545f36d6dd2.png#pic_center)

<a id="5_1_feasibility"></a>
## 5.1 Feasibility of PCA-Kalman
First of all, we evaluated the rate of detection of human activities in three different scenarios. As shown in Table 3, we listed five common behaviors of people in the table. We have chosen a total of three scenarios, as shown in Fig. 2. Furthermore, we com- pare the PCA-Kalman method with the other two methods. Clearly, Fig. 10 shows that the accuracy of PCA-Kalman in LOS, NLOS, and through one wall is much higher than the accuracy of other two methods. And Table 1 lists the detailed data of Fig. 10a. In Fig. 10a, the accuracy of PCA-Kalman declines with the increasing distance; however, the accuracy of PCA-Kalman shown in Fig. 10b, c is not significantly reduced. So, the reason for that is when the tester is far from the receiving end, they are closer to the transmitting end. The signal transmitted increases, but the reflected signal decreases.

> 首先，我们评估了在三种不同场景中检测人类活动的速率。如表3所示，我们列出了表中人们的五种常见行为。我们选择了总共三种场景，如图2所示。此外，我们将PCA-Kalman方法与另外两种方法进行了比较。显然，图10显示，PCA-Kalman在LOS、NLOS和穿过一面墙的精度远高于其他两种方法的精度。而表1列出了图10a的详细数据。在图10a中，随着距离的增加，PCA-Kalman的准确性下降；然而，图10b、c中显示的PCA-Kalman的准确性没有显著降低。因此，原因是当测试者远离接收端时，他们离传输端更近。传输的信号增加，但反射的信号减少。

<a id="5_2_robustness"></a>
## 5.2 Robustness verification
Figure 10d–f shows the detection rate of human ac- tivity in three different scenarios. And Table 2 lists the detailed data of Fig. 10b. In each scene, the dis- tance between the AP and the computer is the same. The results show that the average accuracy of the PCA-Kalman method is 89% in the LOS environment, 87% in the NLOS environment, and 74% in the wall penetration environment. In these three scenarios, the accuracy of the open hall test was high, reaching up to 92, 85, and 77%.

> 图10d-f显示了在三种不同场景中的人类活动检测率。表2列出了图10b的详细数据。在每一个场景中，AP与电脑之间的距离都是相同的。结果显示，在LOS环境中，PCA-Kalman方法的平均准确率为89%，在NLOS环境中为87%，在墙壁穿透环境中为74%。在这三种情境中，开放大厅测试的准确性很高，分别达到92%、85%和77%。

In three scenarios, the average detection rate of meeting rooms was 91, 88, and 75%, respectively. However, in all three scenarios, laboratories have lower detection rates of 84, 81, and 70%, respectively. Furthermore, the average detection rate in the open hall environment and in the meeting room environment is higher than that in the laboratory. The reason is that the laboratory environment has more interference than the other two environments, such as more work desks and office chairs.

> 在三种场景中，会议室的平均检测率分别为91%、88%和75%。但是，在所有三种情境中，实验室的检测率较低，分别为84%、81%和70%。此外，开放大厅环境和会议室环境中的平均检测率高于实验室环境。原因是实验室环境比其他两种环境有更多的干扰，例如更多的工作桌和办公椅。

On the other hand, we detect the activity of a tester at different distances between the transmitter and the receiver. We found that the closer the distance means the higher detection rate. In general, the closer the AP is to the computer, the higher the accuracy is of the results. This is because the reception effect of the Wi-Fi signal received is enhanced with the shortening of the communication distance, thus providing a more reliable CSI feature extraction to capture the different movements of the human body.

> 另一方面，我们检测了在发射机和接收机之间不同距离的测试者的活动。我们发现距离越近意味着检测率越高。一般来说，AP离电脑越近，结果的准确性就越高。这是因为，随着通信距离的缩短，Wi-Fi信号的接收效果得到了增强，从而提供了更可靠的CSI特征提取来捕捉人体的不同动作。

<a id="5_3_the_effect"></a>
## 5.3 The effect of different test areas on the detection

In this section, we discuss the effect of different test areas on the detection rate. In order to detect the effect of the behavior state of different test areas on the detection rate, we divided the experimental area into 20 squares and numbered them in ascending order. Moreover, in this experiment, we have chosen a grid of 1 to 9 for testing. As shown in Fig. 11, obviously, we can see that the detection rate is the highest in the 4, 5, 6, and 7 grid regions. And Table 4 lists the detailed data corresponding to this figure.

> 在这一节中，我们讨论了不同测试区域对检测率的影响。为了检测不同测试区域的行为状态对检测率的影响，我们将实验区域划分为20个方格，并按升序对它们进行了编号。此外，在这个实验中，我们选择了1到9的网格进行测试。如图11所示，显然，我们可以看到4、5、6和7网格区域的检测率是最高的。表4列出了与这个图相对应的详细数据。

![在这里插入图片描述](https://img-blog.csdnimg.cn/476bba70492e431caaa661398a9389d0.png#pic_center)
The main reason is that the distance between the transmitter and the receiver in these three areas is relatively close, and there is no excessive signal inter- ference. However, the detection rates for the first three grids are lower because the three areas are far from the receiving end to the sending end with the interference of multipath effects such as tables and walls next to them. The results show that the detec- tion rate of the PCA-Kalman method is higher than the other two systems.

> 主要原因是这三个区域中发射机和接收机之间的距离相对较近，且没有过多的信号干扰。然而，前三个网格的检测率较低，因为这三个区域从接收端到发送端的距离较远，且旁边有如桌子和墙壁等多径效应的干扰。结果显示，PCA-Kalman方法的检测率高于其他两个系统。

<a id="5_4_detection"></a>
## 5.4 Detection of different performance areas

We focused on the following main metrics to evaluate our detection rate: (a) TP (true positive) for the prob- ability that the human behavior events are correctly detected and (b) FP (false positive) for the fraction of cases in which the system announced a “detected” event when there was no one moving.

> 我们重点关注以下主要指标来评估我们的检测率：(a) TP（真正例）代表人类行为事件被正确检测的概率，以及(b) FP（假正例）代表系统在没有人移动的情况下宣布“检测到”事件的情况比例。

In this section, we compared the detection perform- ance of PCA-Kalman method with R-PMD and FIMD in different test areas. First, we split the test area into 10 small areas from 0 to 9, as shown in Fig. 12. And Table 5 lists the detailed data corresponding to Fig. 12a.

> 在本节中，我们比较了PCA-Kalman方法与R-PMD和FIMD在不同测试区域的检测性能。首先，我们将测试区域划分为从0到9的10个小区域，如图12所示。表5列出了与图12a相对应的详细数据。

Figure 12a shows the TP rates for different test areas. From the figure, we can observe and guess two points:

> 图12a显示了不同测试区域的TP率。从图中，我们可以观察和推测两点：

1. The FIMD method performs well in zones 2, 5, and 8, but shows significant degradation in other areas. This is because 2 and 5 are in the LOS environment, while others are in the NLOS environment. The test TP rate further confirms that FIMD is valid only in the LOS area and is not suitable for detecting random movement patterns of personnel. PCA’s TP rate is far greater than the FIMD over all regions. What is more, the PCA-Kalman method is the best performance. Which further determines the advan- tages of using PCA [49].

> FIMD方法在区域2、5和8表现良好，但在其他区域显示出显著的退化。这是因为2和5位于LOS环境中，而其他区域位于NLOS环境中。进一步的测试TP率证实，FIMD仅在LOS区域有效，不适用于检测人员的随机移动模式。PCA的TP率远高于所有区域的FIMD。更重要的是，PCA-Kalman方法是表现最好的。这进一步确定了使用PCA的优势[49]。

3. We also noticed that PCA achieved excellent results, with the exception of zone 3 testing in all regions, since zone 3 was far from the transmitter and receiver, resulting in less noticeable changes in the signal from people.

> 我们还注意到，除了在所有区域的区域3测试外，PCA取得了出色的结果，因为区域3距离发射机和接收机很远，导致信号中的人员变化不太明显。

Because qualified motion detection systems need to have not only a high-TP rate, but also a low-FP rate. Therefore, the figure shows that the FP rate is also low, and we can clearly observe that the PCA-Kalman method is more stable than the simple PCA method.

> 因为合格的运动检测系统不仅需要有高TP率，而且还需要有低FP率。因此，该图表显示FP率也很低，我们可以清楚地观察到PCA-Kalman方法比简单的PCA方法更稳定。

At the same time, the detection rate is higher than that of FIMD method. The above experimental results confirm that PCA-Kalman is superior to the other two methods. And in the two aspects of TP and FP, comparison can show obvious advantages. Similarly, PCA-Kalman is a more stable method, which can have a higher detection rate in each experimental environment.

> 与此同时，其检测率也高于FIMD方法。上述实验结果证实，PCA-Kalman优于其他两种方法。在TP和FP的两个方面，比较可以显示出明显的优势。同样，PCA-Kalman是一种更稳定的方法，在每个实验环境中都可以获得更高的检测率。

<a id="5_5_impact"></a>
## 5.5 Impact of different sliding window sizes

Since the window size to a certain extent represents the size of the delay, it plays an important role in the overall performance of the detection rate. Additionally, the window size indicator is selected to evaluate the detection rate. Moreover, the window size and performance are proportional. The effect of time variance can be alleviated based on a large amount of data. As can be seen from Fig. 13, the detection rates of the three systems increase with the increase of the window size, but when the window size exceeds a certain set threshold, the time difference of the CSI will be caused by the different behavior states of the personnel. And Table 6 lists the detailed data of Fig. 13.

> 由于窗口大小在一定程度上代表了延迟的大小，所以它在检测率的整体表现中起着重要的作用。此外，选择窗口大小指标来评估检测率。更进一步说，窗口大小和性能是成正比的。基于大量的数据可以减轻时间变化的影响。从图13可以看出，随着窗口大小的增加，三个系统的检测率都在增加，但当窗口大小超过某个设定的阈值时，人员的不同行为状态会导致CSI的时间差异。图13的详细数据列在表6中。

However, when the window is smaller, it is very dif- ficult for the system to detect the human. Because the size of the variance reduces the sensitivity of the system and lowers in a static environment. The results show that the PCA-Kalman method can achieve the best detection performance when the slid- ing window size is set to 10, and the detection rate is relatively stable.

> 然而，当窗口较小时，系统很难检测到人。因为方差的大小降低了系统的灵敏度并在静态环境中降低。结果表明，当滑动窗口大小设置为10时，PCA-Kalman方法可以达到最佳的检测性能，且检测率相对稳定。

![在这里插入图片描述](https://img-blog.csdnimg.cn/4199a070873e46d093bbdd94e3912fcd.png#pic_center)

<a id="5_6_impact_feature"></a>
## 5.6 Impact of feature number

As shown in Fig. 14, when we use more features, it may produce a higher detection rate. Meanwhile, un- like the eigenvalues of the correlation matrix used in the FIMD, its principle is partially focused on the ei- genvalues. However, the covariance matrix we used in the PCA-Kalman method is more dispersed. And Table 7 lists the detailed data corresponding to Fig. 14.

> 如图14所示，当我们使用更多的特征时，可能会产生更高的检测率。同时，与FIMD中使用的相关矩阵的特征值不同，其原理部分集中在特征值上。但是，我们在PCA-Kalman方法中使用的协方差矩阵更为分散。图14的详细数据列在表7中。

Nevertheless, it can be shown from the figure that when different features are used and the number of features increases, the detection rate of PCA-Kalman method proposed in this paper will also increase. Moreover, it is not difficult to see that when the eigenvalue reaches 5, it basically stays steady. In contrast, the turning point for method FIMD was 4 and beyond and the detection rate dropped significantly. The results show that the detection rate of our proposed PCA-Kalman method is more stable.

> 然而，从图中可以看出，当使用不同的特征且特征数量增加时，本文提出的PCA-Kalman方法的检测率也会增加。此外，不难看出，当特征值达到5时，它基本上保持稳定。相比之下，FIMD方法的转折点是4，超过这个值后，检测率显著下降。结果表明，我们提出的PCA-Kalman方法的检测率更稳定。

![在这里插入图片描述](https://img-blog.csdnimg.cn/e1cce9d4eaa340d99440539a6ea221c7.png#pic_center)
![在这里插入图片描述](https://img-blog.csdnimg.cn/7be29f5d6f1343a492d3e95fb65f845d.png#pic_center)

<a id="5_7_impact_packet"></a>
## 5.7 Impact of packet number
We observe that different data packets have a great influence on the detection accuracy of the experi- ment. Moreover, we choose different data packets for testing. As shown in Fig. 15, the true positive (TP) rate increases as the number of packets increases. The true negative (TN) rate is almost unaffected. The reason is that in the absence of a human being, most of the channel measurements remain stable enough for a short period of time to capture the feature.

> 我们观察到不同的数据包对实验的检测准确性有很大的影响。此外，我们选择了不同的数据包进行测试。如图15所示，真阳性(TP)率随着数据包数量的增加而增加。真阴性(TN)率几乎不受影响。原因是在没有人存在的情况下，大部分的信道测量在短时间内都足够稳定，可以捕获到特征。

On the contrary, inadequate samples cannot characterize human time changes because the effects of human movement are not evenly distributed. Overall, both TP and TN rates can reach 90%, so the method described in this article works well.

> 相反，不足的样本不能表征人的时间变化，因为人的移动效应并不均匀分布。总的来说，TP和TN的比率都可以达到90%，所以这篇文章描述的方法效果很好。

<a id="6_conclusion"></a>
# 6 Conclusions

In this paper, we propose a method for detecting indoor activity using ubiquitous Wi-Fi, called PCA-Kalman, and extract CSI signals from commercial off-the-shelf (COTS) Wi-Fi devices. We propose to extract a robust feature from the CSI signal to detect the behavior change of indoor personnel. In order to achieve it, we analyzed the limitations of Kalman filtering technique and PCA to improve denoising performance and capture the representative signals of human behavior. Therefore, the use of CSI is triggered, and the method proposed in this paper can obtain higher detection rate and robust- ness without any equipment. Meanwhile, a method of detecting human behavior without equipment based on CSI is proposed, by applying SVM to solve the human behavior detection problem. And we applied on CSI data to reduce noise and PCA is applied to extract feature and reduce dimensionality.

> 在这篇论文中，我们提出了一种使用无处不在的Wi-Fi检测室内活动的方法，称为PCA-Kalman，并从商用现成的(COTS) Wi-Fi设备中提取CSI信号。我们提议从CSI信号中提取一个稳健的特征，以检测室内人员的行为变化。为了实现这一目标，我们分析了Kalman滤波技术和PCA的局限性，以提高去噪性能并捕获人类行为的代表性信号。因此，使用CSI被触发，并且这篇文章提出的方法可以在没有任何设备的情况下获得更高的检测率和稳健性。同时，提出了一种基于CSI的无设备检测人类行为的方法，通过应用SVM来解决人类行为检测问题。我们在CSI数据上应用了去噪，并应用PCA来提取特征和降维。

Further, the experimental results show that the detec- tion rate of this method is 95% in three different test areas. Therefore, the method has strong robustness and stability. Our repeated tests in different scenarios have been com- pared with other methods. Besides, PCA-Kalman method has a high stability and good detection rate of indoor personnel behavior detection method. However, there are some shortcomings in our approach. For example, when there are multiple people, this will affect our experimental results. Moreover, we will solve the problem in the future. In addition, in this paper, the experiment of this method is carried out in the 2.4 G band. In order to further improve the overall performance and efficiency, in the future, we will explore the performance and efficiency of indoor personnel status detection in the 5 G band.

> 进一步地，实验结果显示，在三个不同的测试区域，这种方法的检测率为95%。因此，该方法具有很强的稳健性和稳定性。我们在不同的场景中进行的重复测试与其他方法进行了比较。此外，PCA-Kalman方法在室内人员行为检测方法中具有高稳定性和良好的检测率。然而，我们的方法也存在一些不足之处。例如，当有多人时，这将影响我们的实验结果。此外，我们将在未来解决这个问题。另外，在这篇论文中，这种方法的实验是在2.4 G频带中进行的。为了进一步提高整体性能和效率，我们将在未来探讨在5 G频带中室内人员状态检测的性能和效率。

**Abbreviations**
AP: Access points; CFR: Channel frequency response; CIR: Channel impulse response; CSI: Channel state information; FP: False positive; LOS: Line-of-sight; MP: Monitoring points; NIC: Network interface card; NLOS: Non-line-of-sight; PCA: Principal component analysis; RSSI: Received signal strength indication; SVM: Support vector machine; TN: True negative; TP: True positive;
UWB: Ultra-wideband; WSNs: Wireless sensor networks

**Acknowledgements**
The authors would like to thank the reviewers for their thorough reviews and helpful suggestions.

**Funding**
This work was supported by the National Natural Science Foundation of China under grant nos. 61363059, 61762079, and 61662070; Key Science and Technology Support Program of Gansu Province under grant nos. 1604FKCA097 and 17YF1GA015; and Science and Technology Innovation Project of Gansu Province under grant nos. 17CX2JA037 and 17CX2JA039.

**Authors’ contributions**
XCD has contributed towards the algorithms and the analysis. As the supervisor of YNH, he has proofread the paper several times and provided guidance throughout the whole preparation of the manuscript. YNH has contributed towards the algorithms, the analysis, and the simulations and wrote the paper. ZJH and XS have revised the equations, helped in writing the introduction and the related works, and critically revised the paper. All authors read and approved the final manuscript.

**Competing interests**
The authors declare that they have no competing interests.

**Publisher’s Note**
Springer Nature remains neutral with regard to jurisdictional claims in published maps and institutional affiliations.

Received: 10 March 2018 Accepted: 20 August 2018
Published online 31 August 2018

<a id="reference"></a>
# References
[1] L. Chen, X. Chen, L. Ni, Y. Peng, D. Fang, Human behavior recognition using Wi-Fi CSI: challenges and opportunities. IEEE Commun. Mag. 55(10), 112– 117 (2017)

[2] X. Wu, Z. Tian, T.N. Davidson, G.B. Giannakis, Optimal waveform design for UWB radios. IEEE Trans. Signal Process. 54(6), 2009–2021 (2004)

[3] L. Gong, W. Yang, Z. Zhou, D. Man, H. Cai, X. Zhou, et al., An adaptive wireless passive human detection via fine-grained physical layer information. Ad Hoc Netw. 38(C), 38–50 (2016)

[4] R. Zhou, X. Lu, P. Zhao, J. Chen, Device-free presence detection and localization with SVM and CSI fingerprinting. IEEE Sensors J. 17(23), 7990- 7999 (2017)

[5] S. Li, Z. Qin, H. Song, X. Yang, R. Zhang, A lightweight and aggregated system for indoor/outdoor detection using smart devices. Futur. Gener. Comput. Syst. (2017). https://doi.org/10.1016/j.future.2017.05.028

[6] O. Ammae, J. Korpela, T. Maekawa, Unobtrusive detection of body movements during sleep using Wi-Fi received signal strength with model adaptation technique. Futur. Gener. Comput. Syst. 78, 616-625. (2018)

[7] A. Chriki, H. Touati, H. Snoussi, SVM-based indoor localization in wireless sensor networks. Wireless communications and mobile computing conference IEEE (pp. 1144-1149) (2017). https://doi.org/10.1109/IWCMC.2017. 7986446

[8] C. Wu, Z. Yang, Z. Zhou, X. Liu, Y. Liu, J. Cao, Non-invasive detection of moving and stationary human with WiFi. IEEE Journal on Selected Areas in Communications 33(11), 2329–2342 (2015)

[9] H. Zhu, F. Xiao, L. Sun, X. Xie, P. Yang, R. Wang, Robust and passive motion detection with cots WiFi devices. Tsinghua Science Technology 22(4), 345– 359 (2017)

[10] F. Xiao, X. Xie, H. Zhu, L. Sun, R. Wang, Invisible Cloak Fails: CSI-Based Passive Human Detection. In Proceedings of the 1st Workshop on Context Sensing and Activity Recognition (ACM, 2015), pp. 19–23

[11] K. Qian, C. Wu, Z. Yang, Y. Liu, K. Jamieson, Widar: Decimeter-Level Passive Tracking via Velocity Monitoring with Commodity Wi-Fi. In Proceedings of the 18th ACM International Symposium on Mobile Ad Hoc Networking and Computing. ACM (2017)

[12] L. Zhang, E. Ding, Z. Zhao, Y. Hu, X. Wang, K. Zhang, A novel fingerprinting using channel state information with MIMO–OFDM. Clust. Comput. 20(4), 1– 14 (2017)

[13] Wang, J., Jiang, H., Xiong, J., Jamieson, K., Chen, X., Fang, D., et al. LiFS: Low Human-Effort, Device-Free Localization with Fine-Grained Subcarrier Information). International Conference on Mobile Computing and NETWORKING ACM (Pp.243–256(2016)

[14] H. Wang, D. Zhang, Y. Wang, J. Ma, Y. Wang, S. Li, RT-fall: a real-time and contactless fall detection system with commodity wifi devices. IEEE Trans. Mob. Comput. 16(2), 511–526 (2017)

[15] J. Xiao, K. Wu, Y. Yi, et al., FIFS: Fine-Grained Indoor Fingerprinting System[C]//Computer Communications and Networks (ICCCN), 2012 21st International Conference on. IEEE, 2012: 1-7.

[16] H. Zhu, F. Xiao, L. Sun, R. Wang, P. Yang, R-TTWD: robust device-free through-the-wall detection of moving human with WiFi. IEEE Journal on Selected Areas in Communications (99), 1–1 (2017)

[17] J. Lv, W. Yang, L. Gong, D. Man, X. Du, Robust WLAN-Based Indoor Fine- Grained Intrusion Detection (IEEE Global Communications Conference, IEEE. pp. 1-6, 2017)

[18] H. Xie, L. Lin, Z. Jiang, W. Xi, K. Zhao, M. Ding, et al., Accelerating Crowdsourcing Based Indoor Localization Using CSI. IEEE, International Conference on Parallel and Distributed Systems IEEE (ICPADS). IEEE, (pp. 274-281)(2016)

[19] S.S. Nanivadekar, U.D. Kolekar, An approach involving dynamic group search optimization for allocating resources in OFDM-based cognitive radio system. Alex. Eng. J. 57(1), 193-202, (2018)

[20] D. Wu, D. Zhang, C. Xu, et al., Device-Free WiFi Human Sensing: From Pattern-Based to Model-Based Approaches[J]. IEEE. Commun. Mag. 55(10), 91-97 (2017)

[21] S. He, J. Chen, X. Li, X. Shen, Y. Sun, Mobility and intruder prior information improving the barrier coverage of sparse sensor networks. IEEE Trans. Mob. Comput. 13(6), 1–1 (2014)

[22] X. Wang, L. Gao, S. Mao, S. Pandey, CSI-based fingerprinting for indoor localization: a deep learning approach. IEEE Trans. Veh. Technol. 66(1), 763– 776 (2017)

[23] Z. Zhou, Z. Yang, C. Wu, L. Shangguan, Y. Liu, Omnidirectional coverage for device-free passive human detection. IEEE Transactions on Parallel & Distributed Systems 25(7), 1819–1829 (2014)

[24] J. Xiao, K. Wu, Y. Yi, L. Wang, L.M. Ni, Fimd: fine-grained device-free motion detection. 90(1), 229–235 (2012)

[25] D. Zhu, N. Pang, G. Li, S. Liu, NotiFi: A ubiquitous WiFi-based abnormal activity detection system. Neural Networks (IJCNN), 2017 International Joint Conference on. IEEE. 1766-1773 (2017)

[26] S. Li, X. Li, K. Niu, H. Wang, Y. Zhang, D. Zhang, AR-Alarm: An Adaptive and Robust Intrusion Detection System Leveraging CSI from Commodity Wi-Fi. International Conference on Smart Homes and Health Telematics Springer. Cham. 10461, 211-223 (2017)

[27] N. Shrivastava, A. Trivedi, Combined beamforming with space-time- frequency coding for MIMO–OFDM systems. AEUE - International Journal of Electronics and Communications 69(6), 878–883 (2015)

[28] H.F. Mahdi, M. Ismail, R. Nordin, Comparison of channel state information estimation using slm and clipping-based papr reduction methods. Procedia Technology 11(1), 955–961 (2013)

[29] C. Xu, Z. Jia, P. Chen, B. Wang, CSI-Based Autoencoder Classification for Wi- Fi Indoor Localization (IEEE Control and Decision Conference, 2016). IEEE, pp. 6523–6528

[30] Y. Zheng, C. Wu, K. Qian, Z. Yang, Y. Liu, Detecting Radio Frequency Interference for CSI Measurements on COTS WiFi Devices. ICC 2017, 2017 IEEE International Conference on Communications (IEEE, pp. 1–6). (2017)

[31] Tzur, A., Amrani, O., Wool, A. Direction finding of rogue Wi-Fi access points using an off-the-shelf MIMO–OFDM receiver. Physical Communication, 17(C), 149–164 (2015)

[32] X. Li, D. Zhang, Q. Lv, J. Xiong, S. Li, Y. Zhang, et al., IndoTrack: device-free indoor human tracking with commodity Wi-Fi. Proceedings of the Acm on Interactive Mobile Wearable & Ubiquitous Technologies 1(3), 72 (2017)

[33] R. Benlamri, M. Sparer, Leadership, Innovation and Entrepreneurship as Driving Forces of the Global Economy[M]. Springer. pp. 297-392 (2017)

[34] A.I. Alomari, A. Gawanmeh, Challenges in formal methods for testing and verificationof cloud computing systems. Scalable Computing Practice & Experience 16(3), 321–332 (2015)

[35] Z. Yang, J. Bhimani, J. Wang, D. Evans, N. Mi, Z. Yang, et al., Automatic and scalable data replication manager in distributed computation and storage infrastructure of cyber-physical systems. Scalable Computing 18(4), 291–311 (2017)

[36] Z. Yang, Y. Wang, J. Bhamini, C.C. Tan, N. Mi, EAD: elasticity aware deduplication manager for datacenters with multi-tier storage systems. Cluster Computing. (12), 1–19 (2018). https://doi.org/10.1007/s10586-018-2141-z

[37] Z. Yang, J. Bhimani, Y. Yao, C.H. Lin, J. Wang, N. Mi, et al., AutoAdmin: automatic and dynamic resource reservation admission control in Hadoop YARN clusters. Scalable Computing. 19(1), 53-68 (2018)

[38] H. Hu, L. Li, A New Method Using Covariance Eigenvalues and Time Window in Passive Human Motion Detection Based on CSI Phases (IEEE In Electromagnetic Compatibility (EMC-Beijing), 2017 IEEE 5th International Symposium, IEEE, 2017), pp 1–6. https://doi.org/10.1109/EMC-B.2017.8260358

[39] R. Zhou, X. Lu, P. Zhao, J. Chen, Device-Free Presence Detection and Localization with SVM and CSI Fingerprinting. IEEE sensors J. 17(23), 7990- 7999 (2017)

[40] Wang, W., Liu, A. X., Shahzad, M., Ling, K., & Lu, S.. Understanding and Modeling of WiFi Signal Based Human Activity Recognition. International Conference on Mobile Computing and Networking ACM (Pp.65–76) (2015)

[41] Y. Wang, K. Wu, L.M. Ni, WiFall: Device-Free Fall Detection by Wireless Networks. IEEE Transactions on Mobile Computing. IEEE. 16(2), 581-594 (2017)

[42] Wang, Y., Liu, J., Chen, Y., Gruteser, M., Yang, J., & Liu, H.. E-eyes: device-free location-oriented activity identification using fine-grained WiFi signatures (2014)

[43] S. D. Domenico, G. Pecoraro, E. Cianea, M. D. Sanctis, Trained-Once Device- Free Crowd Counting and Occupancy Estimation Using WiFi: A Doppler Spectrum Based Approach. IEEE, International Conference on Wireless and Mobile Computing, NETWORKING and Communications IEEE, pp. 1-8 (2016). https://doi.org/10.1109/WiMOB.2016.7763227

[44] X. Zhou, T. Chen, D. Guo, X. Teng, B. Yuan, From one to crowd: a survey on crowdsourcing-based wireless indoor localization. Frontiers of Computer Science 1, 1–28 (2017)

[45] A. Bansal, A.K. Kohli, Suppression of impulsive noise in ofdm system using imperfect channel state information. Optik - International Journal for Light and Electron Optics 127(4), 2111–2115 (2016)

[46] Y. Xie, Z. Li, M. Li, Precise Power Delay Profiling with Commodity WiFi. MobiCom '15 Proceedings of the 21st Annual International Conference on Mobile Computing and Networking, ACM. pp. 53-64 (2015)

[47] F. Deng, S. Guan, X. Yue, X. Gu, J. Chen, J. Lv, et al., Energy-based sound source localization with low power consumption in wireless sensor networks. IEEE Transactions on Industrial Electronics. 64(6), 4894-4902 (2017)

[48] Zhou, Q., Wu, C., Xing, J., Li, J., Yang, Z., & Yang, Q. Wi-Dog: Monitoring School Violence with Commodity WiFi Devices. Wireless Algorithms, Systems, and Applications (2017)

[49] P. Mathecken, T. Riihonen, S. Werner, R. Wichman, Constrained phase noise estimation in ofdm using scattered pilots without decision feedback. IEEE Trans. Signal Process. 65(9), 2348–2362 (2017)