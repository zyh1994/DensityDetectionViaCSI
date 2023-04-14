# Authors
Mingmin Zhao 
Tianhong Li 
Mohammad Abu Alsheikh 
Yonglong Tian 
Hang Zhao
Antonio Torralba 
Dina Katabi


# Abstract
This paper demonstrates accurate human pose estimation through walls and occlusions. We leverage the fact that wireless signals in the WiFi frequencies traverse walls and reflect off the human body. We introduce a deep neural network approach that parses such radio signals to estimate 2D poses. Since humans cannot annotate radio signals, we use state-of-the-art vision model to provide cross-modal supervision. Specifically, during training the system uses synchronized wireless and visual inputs, extracts pose information from the visual stream, and uses it to guide the training process. Once trained, the network uses only the wireless signal for pose estimation. We show that, when tested on visible scenes, the radio-based system is almost as accurate as the vision-based system used to train it. Yet, unlike vision-based pose estimation, the radio-based system can estimate 2D poses through walls despite never trained on such scenarios. Demo videos are available at our website.

> 本文展示了通过墙壁和遮挡准确估计人体姿势的方法。我们利用WiFi频率中的无线信号穿过墙壁并反射到人体的事实。我们引入了一种深度神经网络方法，该方法解析这些射频信号以估计2D姿势。由于人类无法注释射频信号，我们使用最先进的视觉模型提供跨模态监督。具体来说，在训练过程中，系统使用同步的无线和视觉输入，从视觉流中提取姿势信息，并使用它来指导训练过程。一旦训练完成，网络仅使用无线信号进行姿势估计。我们表明，在可见场景测试时，基于无线的系统几乎与用于训练它的视觉基系统一样准确。然而，与基于视觉的姿势估计不同，基于无线的系统可以通过墙壁估计2D姿势，即使从未在这样的情况下训练。演示视频可在我们的网站上找到。

# 1. Introduction

Estimating the human pose is an important task in computer vision with applications in surveillance, activity recognition, gaming, etc. The problem is defined as generating 2D skeletal representations of the joints on the arms and legs, and keypoints on the torso and head. It has recently witnessed major advances and significant performance improvements [30, 27, 28, 46, 31, 20, 10, 16, 33, 12, 47, 37, 45, 13]. However, as in any camera-based recognition task, occlusion remains a fundamental challenge. Past work deals with occlusion by hallucinating the occluded body parts based on the visible ones. Yet, since the human body is deformable, such hallucinations are prone to errors. Further, this approach becomes infeasible when the person is fully occluded, behind a wall or in a different room.

> 估计人体姿势是计算机视觉中一项重要任务，在监控、活动识别、游戏等领域有广泛应用。该问题的定义是生成肢体、躯干和头部的关节的 2D 骨架表示。最近，该领域取得了重大进展并取得了显著的性能提升 [30, 27, 28, 46, 31, 20, 10, 16, 33, 12, 47, 37, 45, 13]。然而，与任何基于摄像机的识别任务一样，遮挡仍然是一个根本性的挑战。过去的工作通过根据可见部分对遮挡部分进行幻觉处理来解决遮挡问题。然而，由于人体可变形，这样的幻觉容易出错。此外，当人被完全遮挡，在墙后或在不同的房间中时，这种方法就不可行了。


This paper presents a fundamentally different approach to deal with occlusions in pose estimation, and potentially other visual recognition tasks. While visible light is easily blocked by walls and opaque objects, radio frequency (RF) signals in the WiFi range can traverse such occlusions. Further, they reflect off the human body, providing an opportunity to track people through walls. Recent advances in wireless systems have leveraged those properties to detect people [5] and track their walking speed through occlusions [19]. Past systems however are quite coarse: they either track only one limb at any time [5, 4], or generate a static and coarse description of the body, where body-parts observed at different time are collapsed into one frame [4]. Use of wireless signals to produce a detailed and accurate description of the pose, similar to that achieved by a stateof-the-art computer vision system, has remained intractable.

> 本文提出了一种处理姿态估计中遮挡的基本不同方法，并可能适用于其他视觉识别任务。尽管可见光容易被墙壁和不透明物体阻挡，但WiFi频率范围内的射频（RF）信号可以穿过这样的遮挡。此外，它们反射人体，提供通过墙壁跟踪人们的机会。无线系统的最新进展已利用这些特性来检测人员[5]并跟踪他们的步行速度[19]。然而，过去的系统非常粗糙：它们只能在任意时间跟踪一条肢体[5,4]，或生成一个静态和粗略的身体描述，其中在不同时间观察到的身体部位被合并到一个帧中[4]。使用无线信号产生细节丰富且精确的姿态描述，类似于现有技术的计算机视觉系统所实现的，仍然是困难的。

In this paper, we introduce RF-Pose, a neural network system that parses wireless signals and extracts accurate 2D human poses, even when the people are occluded or behind a wall. RF-Pose transmits a low power wireless signal (1000 times lower power than WiFi) and observes its reflections from the environment. Using only the radio reflections as input, it estimates the human skeleton. Fig. 1 shows an example output of RF-Pose tracking a person as he enters a room, becomes partially visible through a window, and then walks behind the wall. The RGB images in the top row show the sequence of events and the occlusions the person goes through; the middle row shows the confidence maps of the human keypoints extracted by RF-Pose; and the third row shows the resulting skeletons. Note how our pose estimator tracks the person even when he is fully occluded behind a wall. While this example shows a single person, RF-Pose works with multiple people in the scene, just as a state-of-art vision system would.

> 这篇论文介绍了一种基于无线电频率的解决遮挡问题的方法，即 RF-Pose，一种神经网络系统，能够在人们被遮挡或在墙后时仍然准确提取 2D 人体姿态。 RF-Pose 发送一个功率极低的无线信号（比 WiFi 低 1000 倍），并观察其反射从环境。仅使用无线信号的反射作为输入，它估计人体骨架。图 1 展示了一个 RF-Pose 跟踪一个人进入房间、通过窗户部分可见，然后走到墙后的例子。最上面一排的 RGB 图像展示了人们经历的事件和遮挡；中间一排展示了 RF-Pose 提取的人体关键点的置信度图；最后一排展示了得到的骨架。请注意我们的姿态估计器如何在人完全被墙遮挡时仍然跟踪他。虽然这个例子展示了一个人，但 RF-Pose 可以像最先进的视觉系统一样适用于场景中的多个人。

![在这里插入图片描述](https://img-blog.csdnimg.cn/1ee4d364c09946db85f6d32209fab531.png#pic_center)

*Figure 1: The figure shows a test example with a single person. It demonstrates that our system tracks the pose as the person enters the room and even when he is fully occluded behind the wall. Top: Images captured by a camera colocated with the radio sensor, and presented here for visual reference. Middle: Keypoint confidence maps extracted from RF signals alone, without any visual input. Bottom: Skeleton parsed from keypoint confidence maps showing that we can use RF signals to estimate the human pose even in the presence of full occlusion.*

> 图1：该图展示了一个单独人员的测试示例。它证明了我们的系统可以跟踪人员进入房间时的姿势，即使当他完全被墙壁遮挡。顶部：由与射频传感器同一位置的摄像机捕捉的图像，并在此处用于可视参考。中间：仅从射频信号中提取的关键点置信图，没有任何视觉输入。底部：从关键点置信图解析的骨架，显示我们可以使用射频信号来估计人体姿势，即使存在完全遮挡。

The design and training of our network present different challenges from vision-based pose estimation. In particular, there is no labeled data for this task. It is also infeasible for humans to annotate radio signals with keypoints. To address this problem, we use cross-modal supervision. During training, we attach a web camera to our wireless sensor, and synchronize the the wireless and visual streams. We extract pose information from the visual stream and use it as a supervisory signal for the wireless stream. Once the system is trained, it only uses the radio signal as input. The result is a system that is capable of estimating human pose using wireless signals only, without requiring human annotation as supervision. Interestingly, the RF-based model learns to perform pose estimation even when the people are fully occluded or in a different room. It does so despite it has never seen such examples during training.

> 在本文中，我们介绍了 RF-Pose，这是一个神经网络系统，它可以解析无线信号，即使人们被遮挡或在墙后，也可以提取准确的2D人体姿势。RF-Pose发送低功率无线信号（比WiFi低1000倍的功率）并观察其从环境中反射。仅使用无线反射作为输入，它估计人体骨架。图1显示了 RF-Pose 跟踪一个人进入一个房间，通过窗户部分可见，然后走在墙后的示例输出。顶部行中的RGB图像显示了事件的顺序和遮挡的人；中间行显示由 RF-Pose 提取的人体关键点的置信度图；第三行显示了生成的骨架。请注意，即使他完全被遮挡在墙后，我们的姿势估计器也可以跟踪人。虽然这个例子显示了单个人，但 RF-Pose 与场景中的多个人一样工作，就像最先进的视觉系统一样。我们的网络的设计和训练提出了与基于视觉的姿势估计不同的挑战。特别是，没有为此任务标记的数据。人们标注无线信号的关键点也是不可行的。为了解决这个问题，我们使用跨模

Beyond cross-modal supervision, the design of RF-Pose accounts for the intrinsic features of RF signals including low spatial resolution, specularity of the human body at RF frequencies that traverse walls, and differences in representation and perspective between RF signals and the supervisory visual stream.

> 在跨模态监督之外，RF-Pose的设计还考虑了射频信号的固有特征，包括空间分辨率低、射线穿过墙壁时人体的反射以及射频信号与监督视觉流之间的表示和视角差异。

We train and test RF-Pose using data collected in public environments around our campus. The dataset has hundreds of different people performing diverse indoor activities: walking, sitting, taking stairs, waiting for elevators, opening doors, talking to friends, etc. We test and train on different environments to ensure the network generalizes to new scenes. We manually label 2000 RGB images and use them to test both the vision system and RF-Pose. The results show that on visible scenes, RF-Pose has an average precision (AP) of 62.4 whereas the vision-based system used to train it has an AP of 68.8. For through-wall scenes, RF-Pose has an AP of 58.1 whereas the vision-based system fails completely

> 本文的实验基于我们校园周围的公共环境收集的数据进行训练和测试。数据集包含数百个不同的人在室内进行各种活动：行走，坐着，爬楼梯，等电梯，开门，与朋友交谈等。为了确保网络能够推广到新的场景，我们在不同的环境中进行测试和训练。我们手动标记了2000张RGB图像，并用它们来测试视觉系统和RF-Pose。结果显示，在可见场景中，RF-Pose的平均精度（AP）为62.4，而用于训练它的基于视觉的系统的AP为68.8。对于通过墙壁的场景，RF-Pose的AP为58.1，而基于视觉的系统完全失败。

We also show that the skeleton learned from RF signals extracts identifying features of the people and their style of moving. We run an experiment where we have 100 people perform free walking, and train a vanilla-CNN classifier to identify each person using a 2-second clip of the RF-based skeleton. By simply observing how the RF-based skeleton moves, the classifier can identify the person with an accuracy over 83% in both visible and through wall scenarios.

> 我们还证明了从射频信号中学习的骨架可以提取人们的识别特征以及他们的行动方式。我们进行了一项实验，让100人自由行走，并使用基于射频的骨架的2秒剪辑训练香浓-CNN分类器以识别每个人。仅仅通过观察射频基于骨架的移动，分类器可以在可见和穿过墙的情况下以超过83%的准确率识别该人。

# Related Work

**(a) Computer Vision**: Human pose estimation from RGB images generally falls into two main categories: Top-down and bottom-up methods. Top-down methods [16, 14, 29, 15] first detect each people in the image, and then apply a single-person pose estimator to each people to extract keypoints. Bottom-up methods [10, 31, 20], on the other hand, first detect all keypoints in the image, then use postprocessing to associate the keypoints belonging to the same person. We build on this literature and adopt a bottom-up approach, but differ in that we learn poses from RF signals. While some prior papers use sensors other than conventional cameras, such as RGB-D sensors [50] and Vicon [35], unlike RF signals, those data inputs still suffer from occlusions by walls and other opaque structures.

> **(a) 计算机视觉**：基于RGB图像的人体姿态估计通常分为两类：自上而下法和自下而上法。自上而下法 [16, 14, 29, 15] 首先在图像中检测每个人，然后对每个人应用单人姿态估计器以提取关键点。另一方面，自下而上法 [10, 31, 20] 首先检测图像中的所有关键点，然后使用后处理来关联属于同一人的关键点。我们基于这一文献，采用自下而上的方法，但不同的是我们从射频信号中学习姿态。虽然一些先前的论文使用了除了常规摄像机以外的传感器，如RGB-D传感器 [50] 和 Vicon [35]，但与射频信号不同，那些数据输入仍然遭受由墙壁和其他不透明结构引起的遮挡。

In terms of modeling, our work is related to cross-modal and multi-modal learning that explores matching different modalities or delivering complementary information across modalities [8, 11, 36, 34]. In particular, our approach falls under cross-modal teacher-student networks [8], which transfer knowledge learned in one data modality to another. While past work only transfers category-level discriminative knowledge, our network transfers richer knowledge on dense keypoint confidence maps.

> 在建模方面，我们的工作与跨模态和多模态学习有关，它探索了不同模态之间的匹配和跨模态信息的补充[8,11,36,34]。具体来说，我们的方法属于跨模态教师学生网络[8]，它将在一种数据模态中学到的知识转移到另一种数据模态中。虽然过去的工作只转移类别级别的判别知识，但我们的网络转移了密集的关键点置信图的更丰富的知识。

**(b) Wireless Systems**: Recent years have witnessed much interest in localizing people and tracking their motion using wireless signals. The literature can be classified into two categories. The first category operates at very high frequencies (e.g., millimeter wave or terahertz) [3]. These can accurately image the surface of the human body (as in airport security scanners), but do not penetrate walls and furniture.

> **(b) 无线系统**。近年来，人们对使用无线信号定位人和跟踪人的运动产生了很大兴趣。这些文献可以分为两类 类。第一类是在非常高的频率下工作（如毫米波或太赫兹）[3]。它们可以准确地对人体表面进行成像（如机场安检扫描仪）。安全扫描器），但不能穿透墙壁和家具。

The second category uses lower frequencies, around a few GHz, and hence can track people through walls and occlusions. Such through-wall tracking systems can be divided into: device-based and device-free. Device-based tracking systems localize people using the signal generated by some wireless device they carry. For example, one can track a person using the WiFi signal from their cellphone [44, 24, 40]. Since the tracking is performed on the device not the person, one can track different bodyparts by attaching different radio devices to each of them. On the other hand, device-free wireless tracking systems do not require the tracked person to wear sensors on their body. They work by analyzing the radio signal reflected off the person’s body. However, device-free systems typically have low spatial resolution and cannot localize multiple body parts simultaneously. Different papers either localize the whole body [5, 23], monitor the person’s walking speed [43, 19], track the chest motion to extract breathing and heartbeats [6, 51, 52], or track the arm motion to identify a particular gesture [32, 26]. The closest to our work is a system called RF-Capture which creates a coarse description of the human body behind a wall by collapsing multiple body parts detected at different points in time [4]. None of the past work however is capable of estimating the human pose or simultaneously localizing its various keypoints.

> 第二类使用较低的频率，大约几千兆赫，因此可以通过墙壁和遮挡物追踪人。这种穿墙追踪系统可分为：基于设备的和无设备的。基于设备的跟踪系统利用人们携带的一些无线设备产生的信号对人们进行定位。例如，人们可以利用手机的WiFi信号来追踪一个人[44, 24, 40]。由于追踪是在设备上而不是在人身上进行的，人们可以通过在每个人身上安装不同的无线电设备来追踪不同的身体部位。另一方面，无设备无线追踪系统不需要被追踪者在身上佩戴传感器。它们通过分析人身上反射的无线电信号来工作。它们通过分析从人的身体上反射的无线电信号来工作。然而，无设备系统通常具有较低的空间分辨率，不能同时定位多个身体部位。不同的论文要么定位整个身体[5, 23]，监测人的行走速度[43, 19]，跟踪胸部运动以提取呼吸和心跳[6, 51, 52]，或跟踪手臂运动以识别特定手势[32, 26]。与我们的工作最接近的是一个叫做RF-Capture的系统，该系统通过整理在不同时间点检测到的多个身体部位来创建墙后人体的粗略描述[4]。然而，过去的工作没有一个能够估计人体的姿势或同时定位其各种关键点。


Finally, some prior papers have explored human identification using wireless signals [49, 43, 18]. Past work, however, is highly restrictive in how the person has to move, and cannot identify people from free-form walking.

> 最后，以前的一些论文探讨了使用无线信号进行人的识别[49, 43, 18]。然而，过去的工作在人必须如何移动方面有很大的限制，不能从自由行走中识别人。


# 3. RF Signals Acquisition and Properties

Our RF-based pose estimation relies on transmitting a low power RF signal and receiving its reflections. To separate RF reflections from different objects, it is common to use techniques like FMCW (Frequency Modulated Continuous Wave) and antenna arrays [4]. FMCW separates RF reflections based on the distance of the reflecting object, whereas antenna arrays separate reflections based on their spatial direction. In this paper, we introduce a radio similar to [4], which generates an FMCW signal and has two antenna arrays: vertical and horizontal (other radios are also available [1, 2]). Thus, our input data takes the form of two-dimensional heatmaps, one for each of the horizontal and vertical antenna arrays. As shown in Fig. 2, the horizontal heatmap is a projection of the signal reflections on a plane parallel to the ground, whereas the vertical heatmap is a projection of the reflected signals on a plane perpendicular to the ground (red refers to large values while blue refers to small values). Note that since RF signals are complex numbers, each pixel in this map has a real and imaginary components. Our radio generates 30 pairs of heatmaps per second.

> 我们的基于射频的姿态估计依赖于发送低功率射频信号并接收其反射。为了将不同物体的射频反射分离开来，通常使用FMCW（频率调制连续波）和天线阵列等技术[4]。 FMCW根据反射物体的距离将射频反射分开，而天线阵列根据其空间方向将反射分开。在本文中，我们介绍了一种与[4]类似的无线电，它生成一个FMCW信号，并具有两个天线阵列：垂直和水平（也可以使用其他无线电[1,2]）。因此，我们的输入数据采用二维热图形式，一个用于水平天线阵列，一个用于垂直天线阵列。如图2所示，水平热图是信号反射在与地面平行的平面上的投影，而垂直热图是反射信号在与地面垂直的平面上的投影（红色代表大值，蓝色代表小值）。请注意，由于射频信号是复数，因此该图中的每个像素都具有实部和虚部分。我们的无线电每秒生成30对热图。

It is important to note that RF signals have intrinsically different properties than visual data, i.e., camera pixels.

> 值得注意的是，射频信号与视觉数据（即相机像素）有着本质上的不同属性。

**First**, RF signals in the frequencies that traverse walls have low spatial resolution, much lower than vision data. The resolution is typically tens of centimeters [5, 2, 4], and is defined by the bandwidth of the FMCW signal and the aperture of the antenna array. In particular, our radio has a depth resolution about 10 cm, and its antenna arrays have vertical and horizontal angular resolution of 15 degrees.

> **第一**，穿过墙壁的频率的RF信号具有较低的空间分辨率，远低于视觉数据。分辨率通常为数十厘米[5,2,4]，由FMCW信号的带宽和天线阵列的孔径定义。特别地，我们的无线电有大约10厘米的深度分辨率，其天线阵列具有15度的垂直和水平角分辨率。

![在这里插入图片描述](https://img-blog.csdnimg.cn/a472f7da97f349c78ac6b5db85d2ce58.png#pic_center)
*Figure 2: RF heatmaps and an RGB image recorded at the same time.*

> 图2：射频热图和同时记录的RGB图像。


**Second**, the human body is specular in the frequency range that traverse walls [9]. RF specularity is a physical phenomenon that occurs when the wavelength is larger than the roughness of the surface. In this case, the object acts like a reflector - i.e., a mirror - as opposed to a scatterer. The wavelength of our radio is about 5cm and hence humans act as reflectors. Depending on the orientation of the surface of each limb, the signal may be reflected towards our sensor or away from it. Thus, in contrast to camera systems where any snapshot shows all unoccluded key-points, in radio systems, a single snapshot has information about a subset of the limbs and misses limbs and body parts whose orientation at that time deflects the signal away from the sensor.

> **第二**，人体在穿越墙壁的频率范围内具有镜面性 [9]。射频镜面性是在波长大于表面粗糙度时出现的物理现象。在这种情况下，对象充当反射镜，而不是散射体。我们的无线电的波长约为5厘米，因此人类充当反射器。根据每个肢体的表面的朝向，信号可能被反射到我们的传感器或远离它。因此，与任何快照都显示所有未遮挡关键点的相机系统不同，在无线电系统中，单个快照仅包含关于肢体子集的信息，并且遗漏了在该时间朝向偏离传感器的肢体和身体部位的信息。

**Third**, the wireless data has a different representation (complex numbers) and different perspectives (horizontal and vertical projections) from a camera.

> **第三**，无线数据的表现形式（复数）和视角（水平和垂直投影）与相机不同。

The above properties have implications for pose estimation, and need to be taken into account in designing a neural network to extract poses from RF signals.

> 上述特性对姿势估计有影响，在设计从射频信号中提取姿势的神经网络时需要考虑到这些特性。需要考虑到从射频信号中提取姿势的神经网络。


# 4. Method
Our model, illustrated in Fig. 3, follows a teacher-student design. The top pipeline in the figure shows the teacher network, which provides cross-modal supervision; the bottom pipeline shows the student network, which performs RFbased pose estimation.

> 如图3所示，我们的模型遵循老师-学生设计。图中的顶部流水线显示了教师网络，提供跨模态监督；底部流水线显示了学生网络，执行基于RF的姿态估计。

![在这里插入图片描述](https://img-blog.csdnimg.cn/040ec27fe69742aa8600f0a6a5c42681.png#pic_center)
Figure 3: Our teacher-student network model used in RF-Pose. The upper pipeline provides training supervision, whereas the bottom pipeline learns to extract human pose using only RF heatmaps.

> 图3：我们在RF-Pose中使用的教师-学生网络模型。上面的流水线提供训练监督，而下面的流水线学习如何仅使用RF热图提取人体姿势。

## 4.1. Cross-Modal Supervision
One challenge of estimating human pose from RF signals is the the lack of labelled data. Annotating human pose by looking at RF signals (e.g., Fig. 2) is almost impossible. We address this challenge by leveraging the presence of well established vision models that are trained to predict human pose in images [25, 7].

> 一个从射频信号估计人体姿态的挑战是缺乏标记数据。通过观察射频信号（例如，图2）注释人体姿态几乎是不可能的。我们通过利用已经存在的、被训练为预测图像中人体姿态的视觉模型[25,7]来解决这个挑战。

We design a cross-modal teacher-student network that transfers the visual knowledge of human pose using synchronized images and RF signals as a bridge. Consider a synchronized pair of image and RF signals $(\mathbf I, \mathbf R)$, where $\mathbf R$ denotes the combination of the vertical and horizontal heatmaps, and $\mathbf I$ the corresponding image in Fig. 2. The teacher network $\mathbf T(\cdot)$ takes the images $\mathbf I$ as input and predicts keypoint confidence maps as $\mathbf T(\mathbf I)$. These predicted maps $\mathbf T(\mathbf I)$ provide cross-modal supervision for the student network $\mathbf S(\cdot)$, which learns to predict keypoint confidence maps from the RF signals. In this paper, we adopt the 2D pose estimation network in [10] as the teacher network. The student network learns to predict 14 keypoint confidence maps corresponding to the following anatomical parts of the human body: head, neck, shoulders, elbows, wrists, hips, knees and ankles.

> 我们设计了一个跨模态师生网络，使用同步图像和射频信号作为桥梁，转移视觉人体姿态的知识。考虑一对同步的图像和射频信号 $(\mathbf I, \mathbf R)$，其中$\mathbf R$表示垂直和水平热图的组合，$\mathbf I$表示图2中相应的图像。师生网络$\mathbf T(\cdot)$将图像$\mathbf I$作为输入，并预测关键点置信度图$\mathbf T(\mathbf I)$。这些预测的图$\mathbf T(\mathbf I)$为学生网络$\mathbf S(\cdot)$提供跨模态监督，学生网络学会从射频信号预测关键点置信度图。在本文中，我们采用[10]中的2D姿态估计网络作为师生网络。学生网络学会预测14个关键点置信度图，对应人体的以下解剖部位：头、颈、肩、肘、腕、臀、膝和踝。

The training objective of the student network $\mathbf S(·)$ is to minimize the difference between its prediction $\mathbf S(\mathbf R)$ and the teacher network’s prediction $\mathbf T(\mathbf I)$:

> 学生网络 $\mathbf S(·)$ 的训练目标是最小化它的预测 $\mathbf S(\mathbf R)$ 与教师网络的预测 $\mathbf T(\mathbf I)$ 之间的差异：

$$
\min_{S} \sum_{(\mathbf I, \mathbf R)} L(\mathbf T(\mathbf I), \mathbf S(\mathbf R))
$$

We define the loss as the summation of binary cross entropy loss for each pixel in the confidence maps:

> 我们将损失定义为信心图上每个像素的二进制交叉熵损失的总和：

$$
L(T, S) = - \sum_{c} \sum_{i,j} \mathbf S^{c}_{ij} \log \mathbf T^{c}_{ij} + (1 - \mathbf S^{c}_{ij}) \log (1 - \mathbf T^{c}_{ij})
$$

where $T^{c}_{ij}$ and $S^{c}_{ij}$ are the confidence scores for the $(i, j)$-th pixel on the confidence map $c$.

> 其中 $T^{c}{ij}$ 和 $S^{c}{ij}$ 分别是第 $c$ 个信心图的 $(i,j)$ 像素的置信度得分。


## 4.2. Keypoint Detection from RF Signals

The design of our student network has to take into account the properties of RF signals. As mentioned earlier, the human body is specular in the RF range of interest. Hence, we cannot estimate the human pose from a single RF frame ( a single pair of horizontal and vertical heatmaps) because the frame may be missing certain limbs tough they are not occluded. Further, RF signals have low spatial resolution. Hence, it will be difficult to pinpoint the location of a keypoint using a single RF frame. To deal with these issues, we make the network learn to aggregate information from multiple snapshots of RF heatmaps so that it can capture different limbs and model the dynamics of body movement. Thus, instead of taking a single frame as input, we make the network look at sequences of frames. For each sequence, the network outputs keypoint confidence maps as the number of frames in the input – i.e., while the network looks at a clip of multiple RF frames at a time, it still outputs a pose estimate for every frame in the input.

> 我们的学生网络的设计必须考虑射频信号的特性。如前所述，人体在射频感兴趣的范围内是镜面的。因此，我们不能从单个射频帧（即单对水平和垂直热图）估计人体姿势，因为帧可能缺失某些肢体，即使它们不是被遮挡的。此外，射频信号的空间分辨率很低。因此，使用单个射频帧确定关键点的位置将很困难。为了解决这些问题，我们使网络学会从多个射频热图的快照中聚合信息，以便它可以捕捉不同的肢体并模拟身体运动的动态。因此，我们不是将单帧作为输入，而是使网络查看帧序列。对于每个序列，网络输出关键点置信图，数量与输入中的帧数相同 - 即，当网络同时看到多个射频帧时，它仍会对输入中的每个帧输出一个姿态估计。

We also want the network to be invariant to translations in both space and time so that it can generalize from visible scenes to through-wall scenarios. Therefore, we use spatiotemoral convolutions [22, 39, 42] as basic building blocks for the student networks.

> 我们希望网络具有空间和时间上的平移不变性，以便它能够从可见场景推广到穿墙情形。因此，我们使用空间-时间卷积 [22, 39, 42] 作为学生网络的基本构建块。

Finally, the student network needs to transform the information from the views of RF heatmaps to the view of the camera in the teacher network (see Fig. 2). To do so, the model has to first learn a representation of the information in the RF signal that is not encoded in original spatial space, then decode that representation into keypoints in the view of the camera. Thus, as shown in Fig. 3, our student network has: 1) two RF encoding networks $E_h(\cdot)$ and $E_v(\cdot)$ for horizontal and vertical heatmap streams, and 2) a pose decoding network $D(\cdot)$ that takes a channel-wise concatenation of horizontal and vertical RF encodings as input and predicts keypoint confidence maps. The RF encoding networks uses strided convolutional networks to remove spatial dimensions [48, 41] in order to summarize information from the original views. The pose decoding network then uses fractionally strided convolutional networks to decode keypoints in the camera’s view.

> 最后，学生网络需要将从RF热图视图的信息转换为教师网络中的相机视图（见图2）。因此，模型必须先学习原始空间未编码的RF信号的信息表示，然后将该表示解码为相机视图中的关键点。因此，如图3所示，我们的学生网络有：1）两个用于水平和垂直热图流的RF编码网络$E_h(\cdot)$和$E_v(\cdot)$，以及2）姿态解码网络$D(\cdot)$，它将水平和垂直RF编码的通道连接作为输入，并预测关键点置信度图。RF编码网络使用步长卷积网络以概括原始视图的信息以删除空间维度[48,41]。姿态解码网络然后使用分数步长卷积网络解码相机视图中的关键点。

## 4.3. Implementation and Training

**RF encoding network.** Each encoding network takes 100 frames (3.3 seconds) of RF heatmap as input. The RF encoding network uses 10 layers of $9 \times 5 \times 5$ spatio-temporal convolutions with $1 \times 2 \times 2$ strides on spatial dimensions every other layer. We use batch normalization [21] followed by the ReLU activation functions after every layer.

> RF 编码网络。 每个编码网络以 100 帧 (3.3 秒) 的 RF 热图作为输入。RF 编码网络使用 10 层 $9 \times 5 \times 5$ 空间-时间卷积，每隔一层以 $1 \times 2 \times 2$ 步幅在空间维度上进行卷积。我们在每一层后使用批量归一化 [21] 和 ReLU 激活函数。

**Pose decoding network.** We combine spatio-temporal convolutions with fractionally strided convolutions to decode the pose. The decoding network has 4 layers of $3 \times 6 \times 6$ with fractionally stride of $1 \times \frac{1}{2} \times \frac{1}{2}$, except the last layer has one of $1 \times \frac{1}{4} \times \frac{1}{4}$. We use Parametric ReLu [17] after each layer, except for the output layer, where we use sigmoid. 

> 网络解码： 我们结合了空时卷积与分数步长卷积来解码姿态。解码网络共有 4 层 $3 \times 6 \times 6$ 卷积，每层步长分别为 $1 \times \frac{1}{2} \times \frac{1}{2}$，除了最后一层的步长为 $1 \times \frac{1}{4} \times \frac{1}{4}$。除了输出层外，每层后都使用 Parametric ReLu [17] 激活函数，输出层使用 sigmoid 激活函数。

**Training Details**. We represent a complex-valued RF heatmap by two real-valued channels that store the real and imaginary parts. We use a batch size of 24. Our networks are implemented in PyTorch.

> 训练详情： 我们通过两个实值通道表示复数值 RF 热图。批处理大小为 24。我们的网络用 PyTorch 实现。

## 4.4. Keypoint Association

The student network generates confidence maps for all keypoints of all people in the scene. We map the keypoints to skeletons as follows. We first perform non-maximum suppression on the keypoint confidence maps to obtain discrete peaks of keypoint candidates. To associate keypoints of different persons, we use the relaxation method proposed by Cao et al. [10] and we use Euclidean distance for the weight of two candidates. Note that we perform association on a frame-by-frame basis based on the learned keypoint confidence maps. More advanced association methods are possible, but outside the scope of this paper.

> 学生网络为场景中所有人的所有关键点生成置信图。我们将关键点映射到骨架如下：我们首先对关键点置信图执行非最大抑制，以获得关键点候选人的离散峰值。为了关联不同人的关键点，我们使用曹等人提出的松弛方法[10]，并使用欧几里得距离作为两个候选者的权重。请注意，我们根据学习的关键点置信图在帧框架基础上执行关联。更高级的关联方法是可能的，但不在本文范围内。

# 5. Dataset
We collected synchronized wireless and vision data. We attached a web camera to our RF sensor and synchronized the images and the RF data with an average synchronization error of 7 milliseconds.

> 我们收集了同步的无线和视觉数据。我们在 RF 传感器上附加了一个网络摄像头，并使用平均同步误差为 7 毫秒的图像和 RF 数据进行同步。

We conducted more than 50 hours of data collection experiments from 50 different environments (see Fig. 4), including different buildings around our campus. The environments span offices, cafeteria, lecture and seminar rooms, stairs, and walking corridors. People performed natural everyday activities without any interference from our side. Their activities include walking, jogging, sitting, reading, using mobile phones and laptops, eating, etc. Our data includes hundreds of different people of varying ages. The maximum and average number of people in a single frame are 14 and 1.64, respectively. A data frame can also be empty, i.e., it does not include any person. Partial occlusions, where parts of the human body are hidden due to furniture and building amenities, are also present. Legs and arms are the most occluded parts.

> 我们进行了超过 50 个小时的数据收集实验，从 50 个不同的环境中收集（见图 4），包括我们校园周围的不同建筑物。这些环境包括办公室、餐厅、讲座和研讨室、楼梯、行走的走廊等。人们进行自然的日常活动，不受我们的干扰。他们的活动包括走路、慢跑、坐着、阅读、使用手机和笔记本电脑、吃饭等。我们的数据包括数百个不同年龄的人。单帧中最多和平均人数分别为 14 和 1.64。数据帧也可以为空，即不包含任何人。部分遮挡，即由于家具和建筑设施而隐藏人体的部分，也存在。腿和手臂是最多被遮挡的部分。

![在这里插入图片描述](https://img-blog.csdnimg.cn/16a9f64ed7984a49bb4cefea33106ed3.png#pic_center)
*Figure 4: Different environments in the dataset.*

> 图4：数据集中的不同环境。

To evaluate the performance of our model on throughwall scenes, we build a mobile camera system that has 8 cameras to provide ground truth when the people are fully occluded. After calibrating the camera system, we construct 3D poses of people and project them on the view of the camera colocated with RF sensor. The maximum and average number of people in each frame in the through-wall testing set are 3 and 1.41, respectively. This through-wall data was only for testing and was not used to train the model.

> 为了评估我们模型在穿墙场景中的性能，我们构建了一个具有8个摄像头的移动相机系统，以在人被完全遮挡时提供地面真实数据。在校准相机系统之后，我们构建了人的3D姿态，并将它们投影到与RF传感器共同配置的相机视图上。穿墙测试集中每帧中人的最大数量和平均数量分别为3和1.41。这个穿墙数据只用于测试，不用于训练模型。

# 6. Experiments

RF-Pose is trained with 70% of the data from visible scenes, and tested with the remaining 30% of the data from visible scenes and all the data from through-wall scenarios. We make sure that the training data and test data are from different environments.

> 我们使用70%的可见场景数据训练RF-Pose，并使用剩余30%的可见场景数据和所有穿墙场景数据进行测试。我们确保训练数据和测试数据来自不同的环境。

## 6.1. Setup

**Evaluation Metrics**: Motivated by the COCO keypoints evaluation [25] and as is common in past work [10, 29, 16], we evaluate the performance of our model using the average
precision over different object keypoint similarity (OKS). We also report $AP^{50}$ and $AP^{75}$, which denote the average precision when OKS is 0.5 and 0.75, and are treated as loose and strict match of human pose, respectively. We also report AP, which is the mean average precision over 10 different OKS thresholds ranging from 0.5 to 0.95.

> 评估指标：我们以COCO关键点评估[25]为动机，并且和过去的工作[10,29,16]一样，我们使用不同的对象关键点相似度（OKS）的平均精度来评估我们模型的性能。我们还报告$AP^{50}$和$AP^{75}$，分别表示OKS为0.5和0.75时的平均精度，它们被视为人体姿势的宽松和严格匹配。我们还报告AP，这是10个不同OKS阈值（从0.5到0.95）的平均平均精度。

**Baseline**: For visible and partially occluded scenes, we compare RF-Pose with OpenPose [10], a state-of-the-art vision-based model, that also acts as the teacher network.

> 基线：对于可见和部分遮挡的场景，我们将RF-Pose与OpenPose [10]进行比较，OpenPose是一种基于视觉的模型，也是教师网络。

**Ground Truth**: For visible scenes, we manually annotate human poses using the images captured by the camera colocated with our RF sensor. For through-wall scenarios where the colocated camera cannot see people in the other room, we use the eight-camera system described in 5 to provide ground truth. We annotate the images captured by all eight cameras to build 3D human poses and project them on the view of the camera colocated with the radio. We annotate 1000 randomly sampled images from the visible-scene test set and another 1000 examples from the through-wall data.

> 地面实况：对于可见场景，我们使用与我们的RF传感器同位的摄像机捕捉的图像手动标注人体姿势。对于同一房间中的摄像机无法看到其他房间中的人的透过墙场景，我们使用5中描述的八个摄像机系统提供地面实况。我们标记所有八个摄像机捕捉的图像，以构建3D人体姿势，并将它们投影到与无线电同位的摄像机的视图上。我们标记可见场景测试集中的1000

![在这里插入图片描述](https://img-blog.csdnimg.cn/8e16a33f7d6d4614ba894cdcc1f2e3ad.png#pic_center)
*Table 2: Average precision of different keypoints in visible scenes.*

> 表2：可见场景中不同关键点的平均精度。

## 6.2. Multi-Person Pose Estimation Results

We compare human poses obtained via RF signals with the corresponding poses obtained using vision data. Table 1 shows the performance of RF-Pose and the baseline when tested on both visible scenes and through-wall scenarios. The table shows that, when tested on  visible scenes, RF-Pose is almost as good as the vision-based OpenPose that was used to train it. Further, when tested on throughwall scenarios, RF-Pose can achieve good pose estimation while the vision-based baseline completely fail due to occlusion.

> 我们将通过射频信号获得的人体姿态与使用视觉数据获得的姿态进行比较。表1显示了在可见场景和穿墙场景下，RF-Pose和基线的性能。表格显示，在可见场景下测试时，RF-Pose与基于视觉的OpenPose几乎一样好，OpenPose是用来训练它的。此外，在穿墙场景下测试时，RF-Pose可以实现良好的姿态估计，而基于视觉的基线由于遮挡而完全失败。

The performance of RF-Pose on through-wall scenarios can be surprising because the system did not see such examples during training. However, from the perspective of radio signals, a wall simply attenuates the signal power, but maintains the signal structure. Since our model is space invariant, it is able to identify a person behind a wall as similar to the examples it has seen in the space in front of a wall.

> 对于穿墙场景的RF-Pose的表现可能令人惊讶，因为系统在训练过程中没有看到这样的例子。然而，从无线电信号的角度来看，墙壁仅衰减了信号功率，但保持了信号结构。由于我们的模型是空间不变的，因此它可以将墙后的人与它在墙前看到的空间类似的例子相识别。

An interesting aspect in Table 1 is that RF-Pose outperforms OpenPose for $AP^{50}$, and becomes worse at $AP^{75}$. To further explore this aspect, we plot in Fig. 5 the average precision as a function of OKS values. The figure shows that at low OKS values (< 0.7), our model outperforms the vision baseline. This is because RF-Pose predicts less false alarm than the vision-based solution, which can generate fictitious skeletons if the scene has a poster of a person, or a human reflection in a glass window or mirror. In contrast, at high OKS values (> 0.75), the performance of RF-Pose degrades fast, and becomes worse than visionbased approaches. This is due to the intrinsic low spatial resolution of RF signals which prevents them from pinpointing the exact location of the keypoints. The ability of RF-Pose to exactly locate the keypoints is further hampered by imperfect synchronization between the RF heatmaps and the ground truth images.

> 在表1中，RF-Pose超越OpenPose的一个有趣的方面是$ AP^{50} $，并在$ AP^{75} $处变劣。为了进一步探索这个方面，我们在图5中绘制了随OKS值变化的平均精度图。该图显示，在低OKS值（<0.7）时，我们的模型优于视觉基线。这是因为RF-Pose预测的误报比视觉解决方案少，如果场景中有一个人的海报或玻璃窗或镜子中的人的反射，则视觉解决方案可以生成虚构的骨架。相反，在高OKS值（> 0.75）时，RF-Pose的性能快速降低，变得比基于视觉的方法更差。这是由于RF信号的内在低空间分辨率，防止它们确切定位关键点。 RF-Pose确切定位关键点的能力还受到RF热图与地面实况图像之间不完美同步的影响。

Next, we zoom in on the various keypoints and compare their performance. Table 2 shows the average precision of RF-Pose and the baseline in localizing different body parts including head, right and left shoulders, elbows, wrists, hips, knees, and ankles. The results indicate that RF signals are highly accurate at localizing the head and torso (neck and hips) but less accurate in localizing limbs. This is expected because the amount of RF reflections depends on the size of the body part. Thus, RF-Pose is better at capturing the head and torso, which have large reflective areas and relatively slow motion in comparison to the limbs. As for why RF-Pose outperforms OpenPose on some of the keypoints, this is due to the RF-based model operating over a clip of a few seconds, whereas the OpenPose baseline operates on individual images.

> 接下来，我们关注各个关键点并进行比较。表2显示了RF-Pose和基线在定位不同身体部位（包括头部、右和左肩、肘、手腕、臀部、膝盖和脚踝）的平均精度。结果表明，RF信号在定位头部和躯干（颈部和臀部）方面非常准确，但在定位四肢方面不太准确。这是预期的，因为RF反射的数量取决于身体部位的大小。因此，RF-Pose更擅长捕捉具有大反射面积且相对于四肢运动较缓慢的头部和躯干。至于RF-Pose为什么在某些关键点上优于OpenPose，这是因为基于RF的模型是在几秒钟的剪辑上操作的，而OpenPose基线是在单独的图像上操作的。

![在这里插入图片描述](https://img-blog.csdnimg.cn/485b7ed3430d4fa597488c91bf53dc2d.png#pic_center)
*Figure 6: Pose estimation on different activities and environments. First row: Images captured by a web camera (shown as a visual reference). Second row: Pose estimation by our model using RF signals only and without any visual input. Third row: Pose estimation using OpenPose based on images from the first row.*

> 图6：对不同活动和环境的姿势估计。第一行。由网络摄像机拍摄的图像（作为视觉参考显示）。第二行。我们的模型只使用射频信号，没有任何视觉输入，进行姿势估计。第三行。基于第一行的图像，使用OpenPose进行姿势估计*

![在这里插入图片描述](https://img-blog.csdnimg.cn/0352d74b52e64239b90001f3787a0351.png#pic_center)
*Figure 7: Common failure examples. First row: Images captured by a web camera (shown as a visual reference). Second row: Pose estimation by our model using RF signals only and without any visual input. Third row: Pose estimation using OpenPose based on images from the first row.*

> 图7：常见的失败例子。第一行。由网络摄像机拍摄的图像（作为视觉参考显示）。第二行。我们的模型只使用射频信号，没有任何视觉输入，进行姿势估计。第三行。基于第一行的图像，使用OpenPose进行姿势估计。

Finally, we show a few test skeletons to provide a qualitative perspective. Fig. 6 shows sample RF-based skeletons from our test dataset, and compares them to the corresponding RBG images and OpenPose skeletons. The figure demonstrates RF-Pose performs well in different environments with different people doing a variety of everyday activities. Fig. 7 illustrates the difference in errors between RF-Pose and vision-based solutions. It shows that the errors in vision-based systems are typically due to partial occlusions, bad lighting [^1], or confusing a poster or wall-picture as a person. In contrast, errors in RF-Pose happen when a person is occluded by a metallic structure (e.g., a metallic cabinet in Fig. 7(b)) which blocks RF signals, or when people are too close and hence the low resolution RF signal fails to track all of them.

> 最后，我们展示了几个测试骨骼来提供定性角度。图6显示了我们测试数据集中的一些基于RF的骨骼样例，并将其与相应的RBG图像和OpenPose骨骼进行了比较。图表证明RF-Pose在不同环境中表现良好，不同的人正在进行各种日常活动。图7说明了RF-Pose和基于视觉的解决方案之间的误差差异。它表明基于视觉的系统的错误通常是由于部分遮挡，差的照明，或将海报或墙壁图片误认为是一个人造成的。相比之下，RF-Pose中的错误发生在一个人被金属结构（例如，图7（b）中的金属柜）遮挡时，阻塞RF信号，或当人们太接近而低分辨率的RF信号无法跟踪所有人时。

[^1]  Images with bad lighting are excluded during training and testing.

## 6.3. Model Analysis
We use guided back-propagation [38] to visualize the gradient with respect to the input RF signal, and leverage the information to provide insight into our model.

> 最后，我们通过使用引导反向传播（guided back-propagation）[38]来可视化输入射频信号的梯度，从而深入了解我们的模型。

![在这里插入图片描述](https://img-blog.csdnimg.cn/b0fc79e67c5940b1b0154cdb9aa5cc4d.png#pic_center)
*Figure 8: Attention of the model across space*

> 图8：模型跨空间的注意力

**Which part of the RF heatmap does RF-Pose focus on?** Fig. 8 presents an example where one person is walking in front of the wall while another person is hidden behind it. Fig. 8(c) shows the raw horizontal heatmap. The two large boxes are the rescaled versions of the smaller boxes and zoom in on the two people in the figure. The red patch indicated by the marker is the wall, and the other patches are multipath effects and other objects. The gradient in Fig. 8(d) shows that RF-Pose has learned to focus its attention on the two people in the scene and ignore the wall, other objects, and multipath.

> **RF-Pose 的关注点在哪一部分 RF 热图上？** 图 8 展示了一个人在墙前行走，而另一个人隐藏在墙后的例子。图 8(c) 显示了原始的水平热图。两个大框是较小框的缩放版本，着重关注图中的两个人。标记的红色区域表示墙，其他区域表示多径效应和其他物体。图 8(d) 的梯度显示 RF-Pose 已经学会了关注场景中的两个人，并忽略了墙、其他物体和多径。

![在这里插入图片描述](https://img-blog.csdnimg.cn/10f3962d73154e7dae5be79339216c67.png#pic_center)
*Figure 9: Activation of different keypoints over time.*

> 图9：不同关键点随着时间的推移而被激活。

**How does RF-Pose deal with specularity?** Due to the specularity of the human body, some body parts may not reflect much RF signals towards our sensor, and hence may be deemphasized or missing in some heatmaps, even though they are not occluded. RF-Pose deals with this issue by taking as input a sequences of RF frames (i.e., a video clip RF heatmaps). To show the benefit of processing sequences of RF frames, we sum up the input gradient in all pixels in the heatmaps to obtain activation per RF frame. We then plot in Fig. 9 the activation as a function of time to visualize the contribution of each frame to the estimation of various keypoints. The figure shows: that the activations of the right knee (RKnee) and right ankle (RAnkle) are highly correlated, and have peaks at time $t_1$ and $t_2$ when the person is taking a step with her right leg. In contrast, her left wrist (LWrist) gets activated after she raises her forearm at $t_3$, whereas her left elbow (LElbow) remains silent until $t_4$ when she raises her backarm.

> **RF-Pose如何应对镜面效应？** 由于人体的镜面效应，一些身体部位可能不会对我们的传感器反射很多的射频信号，因此它们可能在某些热图中被弱化或丢失，即使它们没有被遮挡。 RF-Pose通过输入射频帧序列（即，射频热图的视频剪辑）来解决这个问题。为了展示处理射频帧序列的好处，我们将热图中所有像素的输入梯度相加，以获得每个射频帧的激活。然后，我们在图9中绘制激活与时间的函数，以可视化各帧对各关键点估计的贡献。该图显示：右膝盖（RKnee）和右踝（RAnkle）的激活高度相关，在人右腿走路时，在$t_1$和$t_2$有峰值。相比之下，她的左腕（LWrist）在$t_3$抬起前臂时被激活，而她的左肘（LElbow）直到$t_4$抬起后臂时才保持沉默。

![在这里插入图片描述](https://img-blog.csdnimg.cn/b2fb618f0e9e4ac59afd008fc3d543c3.png#pic_center)
*Table 3: Average precision of pose estimation trained on varying lengths of input frames.*

> 表3：在不同长度的输入帧上训练的姿势估计的平均精度。

Fig. 9 shows that, for a single output frame, different RF frames in the input sequence contribute differently to the output keypoints. This emphasizes the need for using a sequence of RF frames at the input. But how many frames should one use? Table 3 compares the model’s performance for different sequence length at the input. The average precision is poor when the inout uses only 6 RF frames and increases as the sequence length increases.

> 图9显示，对于单个输出帧，输入序列中的不同射频帧对输出关键点的贡献不同。这强调了在输入中使用射频帧序列的必要性。但是应该使用多少帧呢？表3比较了不同输入序列长度时模型的性能。当输入仅使用6个射频帧时，平均精度很差，随着序列长度的增加而增加。

**But how much temporal information does RF-Pose need?** Given a particular output frame, $i$, we compute the contributions of each of the input frames to it as a function of their time difference from $i$. To do so, we back-propagate the loss of a single frame $w.r.t.$ to the RF heatmaps before it and after it, and sum up the spatial dimensions. Fig. 10 shows the results, suggesting that RF-Pose leverages RF heatmaps up to 1 second away to estimate the current pose.

> **但RF-Pose需要多少时间信息？** 给定一个特定的输出帧，$i$，我们计算每个输入帧对它的贡献，作为它们与$i$时间差的函数。为了做到这一点，我们将单个帧的损失$w.r.t.$反向传播到它之前和之后的RF热图上，并将空间维度相加。图10显示了结果，表明RF-Pose利用1秒以外的RF热图来估计当前姿势。

![在这里插入图片描述](https://img-blog.csdnimg.cn/71f184e954c946a5a1ec268e0f4bfe1c.png#pic_center)
*Figure 10: Contribution of the neighbor to the current frame.*

> 图10：邻居对当前帧的贡献。

## 6.4.  Identification Using RF-Based Skeleton

We would like to show that the skeleton generated by RF-Pose captures personalized features of the individuals in the scene, and can be used by various recognition tasks. Thus, we experiment with using the RF-based skeleton for person identification.

> 我们希望证明RF-Pose生成的骨架能够捕捉场景中个体的个性化特征，并可以用于各种识别任务。因此，我们尝试使用基于RF的骨架进行人员识别。

We conduct person identification experiment with 100 people in two settings: visible environment, where the subject and RF device are in the same room, and through-wall environment, where the RF device captures the person’s reflections through a wall. In each setting, every person walks naturally and randomly inside the area covered by our RF device, and we collect 8 and 2 minutes data separately for training and testing. The skeleton heatmaps are extracted by the model trained on our pose estimation dataset, which never overlaps with the identification dataset. For each setting, we train a 10-layer vanilla CNN to identify people based on 50 consecutive frames of skeleton heatmaps.

> 我们进行了100人的人员识别实验，在两种环境下进行：可见环境，主体和RF设备在同一房间；穿墙环境，RF设备通过墙面捕捉人的反射。在每种环境中，每个人在我们的RF设备覆盖的区域内自然随机地行走，我们分别收集8分钟和2分钟的数据作为训练和测试。骨架热图由在我们的姿态估计数据集上训练的模型提取，这与识别数据集不重叠。对于每种环境，我们训练了一个10层的简单CNN，以根据50个连续的骨架热图帧识别人。

![在这里插入图片描述](https://img-blog.csdnimg.cn/961ae9f4a5b84ac6a8bbf888bdd7b548.png#pic_center)
*Table 4: Top1 and top3 identification percent accuracy in visible and through-wall settings.*

> 表4：在可见和穿墙设置中，Top1和Top3的识别精度百分比。

Table 4 shows that RF-based skeleton identification can reach 83.4% top1 accuracy in visiable scenes. Interestingly, even when a wall blocks the device and our pose extractor never sees these people and such environments during training, the extracted skeletons can still achieve 84.4% top1 accuracy, showing its robustness and generalizability regardless of the wall. As for top3 accuracy, we achieve more than 96% in both settings, demonstrating that the extracted skeleton can preserve most of the discriminative information for identification even though the pose extractor is never trained or fine-tuned on the identification task.

> 表 4 显示 RF 基于骨架的识别在可见场景中可以达到 83.4% 的 top1 准确率。有趣的是，即使当墙壁阻挡了设备并且我们的姿势提取器在训练中从未见过这些人和环境，提取的骨架仍然可以实现 84.4% 的 top1 准确率，这说明它的鲁棒性和泛化性不受墙壁的影响。对于 top3 准确率，我们在两种环境中都达到了 96% 以上，这说明即使姿势提取器没有在识别任务上进行训练或微调，提取的骨架仍然可以保留大部分的判别信息。

# 7.  Scope & Limitations
RF-Pose leverages RF signals to infer the human pose through occlusions. However, RF signals and the solution that we present herein have some limitations: First, the human body is opaque at the frequencies of interest – i.e., frequencies that traverse walls. Hence, inter-person occlusion is a limitation of the current system. Second, the operating distance of a radio is dependent on its transmission power. The radio we use in this paper works up to 40 feet. Finally, we have demonstrated that our extracted pose captures identifying features of the human body. However, our identification experiments consider only one activity: walking. Exploring more sophisticated models and identifying people in the wild while performing daily activities other than walking is left for future work.

> RF-Pose利用RF信号穿过遮挡来推断人体姿态。但是，RF信号和我们在本文中提出的解决方案都存在一些局限性：首先，人体在有关频率（即能穿过墙壁的频率）处是不透明的。因此，人与人之间的遮挡是当前系统的局限性。其次，无线电的操作距离取决于其发射功率。本文中使用的无线电可以工作到40英尺。最后，我们已经证明了我们提取的姿态能够捕捉人体的识别特征。然而，我们的识别实验仅考虑了一项活动：行走。在未来的工作中，将探索更复杂的模型，并在除了行走以外的日常活动中识别人们。

# 8. Conclusion
Occlusion is a fundamental problem in human pose estimation and many other vision tasks. Instead of hallucinating missing body parts based on visible ones, we demonstrate a solution that leverages radio signals to accurately track the 2D human pose through walls and obstructions. We believe this work opens up exciting research opportunities to transfer visual knowledge about people and environments to RF signals, providing a new sensing modality that is intrinsically different from visible light and can augment vision systems with powerful capabilities.

> 遮挡是人体姿态估计和许多其他视觉任务中的基本问题。我们展示了一种利用无线电信号准确跟踪2D人体姿态的解决方案，而不是根据可见的身体部分想象缺失的身体部分。我们相信这项工作为将人和环境的视觉知识转移到RF信号的研究开启了令人兴奋的机会，提供了一种本质上不同于可见光的新传感方式，可以增强视觉系统的强大功能。

**Acknowledgments**: We are grateful to all the human subjects for their contribution to our dataset. We thank the NETMIT members and the anonymous reviewers for their
insightful comments.

# References
[1] Texas instruments. http://www.ti.com/. 3
[2] Walabot. https://walabot.com/. 3
[3] Reusing 60 ghz radios for mobile radar imaging. In In Proceedings of the 21st Annual International Conference on Mobile Computing and Networking, 2015. 2
[4] F. Adib, C.-Y. Hsu, H. Mao, D. Katabi, and F. Durand. Capturing the human figure through a wall. ACM Transactions on Graphics, 34(6):219, November 2015. 1, 3
[5] F. Adib, Z. Kabelac, D. Katabi, and R. C. Miller. 3D tracking via body radio reflections. In Proceedings of the USENIX Conference on Networked Systems Design and Implementation, NSDI, 2014. 1, 3
[6] F. Adib, H. Mao, Z. Kabelac, D. Katabi, and R. C. Miller. Smart homes that monitor breathing and heart rate. In Proceedings of the 33rd Annual ACM Conference on Human Factors in Computing Systems, 2015. 3
[7] M. Andriluka, L. Pishchulin, P. Gehler, and B. Schiele. 2D human pose estimation: New benchmark and state of the art analysis. In Proceedings of the IEEE Conference on computer Vision and Pattern Recognition, pages 3686–3693, 2014. 3
[8] Y. Aytar, C. Vondrick, and A. Torralba. Soundnet: Learning sound representations from unlabeled video. In Advances in Neural Information Processing Systems, NIPS, 2016. 2
[9] P. Beckmann and A. Spizzichino. The scattering of electromagnetic waves from rough surfaces. Norwood, MA, Artech House, Inc., 1987. 3
[10] Z. Cao, T. Simon, S.-E. Wei, and Y. Sheikh. Realtime multiperson 2D pose estimation using part affinity fields. In Proceedings of the IEEE Conference on Computer Vision and Pattern Recognition, CVPR, 2017. 1, 2, 4, 5, 6
[11] L. Castrejon, Y. Aytar, C. Vondrick, H. Pirsiavash, and A. Torralba. Learning aligned cross-modal representations from weakly aligned data. In Proceedings of the IEEE Conference on Computer Vision and Pattern Recognition, CVPR, 2016. 2
[12] X. Chu, W. Ouyang, H. Li, and X. Wang. Structured feature learning for pose estimation. In CVPR, 2016. 1 
[13] X. Chu, W. Yang, W. Ouyang, C. Ma, A. L. Yuille, and X. Wang. Multi-context attention for human pose estimation. In CVPR, 2017. 1
[14] H. Fang, S. Xie, and C. Lu. Rmpe: Regional multi-person pose estimation. arXiv preprint arXiv:1612.00137, 2016. 2
[15] G. Gkioxari, B. Hariharan, R. Girshick, and J. Malik. Using k-poselets for detecting people and localizing their keypoints. In Proceedings of the IEEE Conference on Computer Vision and Pattern Recognition, CVPR, 2014. 2
[16] K. He, G. Gkioxari, P. Dollar, and R. Girshick. Mask r-cnn. In Proceedings of the IEEE international conference on computer vision, ICCV, 2017. 1, 2, 5
[17] K. He, X. Zhang, S. Ren, and J. Sun. Delving deep into rectifiers: Surpassing human-level performance on imagenet classification. In Proceedings of the IEEE international conference on computer vision, ICCV, 2015. 5
[18] F. Hong, X. Wang, Y. Yang, Y. Zong, Y. Zhang, and Z. Guo. Wfid: Passive device-free human identification using WiFi signal. In Proceedings of the 13th International Conference on Mobile and Ubiquitous Systems: Computing, Networking and Services. ACM, 2016. 3
[19] C.-Y. Hsu, Y. Liu, Z. Kabelac, R. Hristov, D. Katabi, and C. Liu. Extracting gait velocity and stride length from surrounding radio signals. In Proceedings of the 2017 CHI Conference on Human Factors in Computing Systems, CHI, 2017. 1, 3
[20] E. Insafutdinov, L. Pishchulin, B. Andres, M. Andriluka, and B. Schiele. Deepercut: A deeper, stronger, and faster multiperson pose estimation model. In Proceedings of the European Conference on Computer Vision, ECCV, 2016. 1, 2
[21] S. Ioffe and C. Szegedy. Batch normalization: Accelerating deep network training by reducing internal covariate shift. In Proceedings of the 32nd International Conference on Machine Learning, ICML, 2015. 5
[22] S. Ji, W. Xu, M. Yang, and K. Yu. 3D convolutional neural networks for human action recognition. IEEE transactions on pattern analysis and machine intelligence, January 2013. 4
[23] K. R. Joshi, D. Bharadia, M. Kotaru, and S. Katti. Wideo: Fine-grained device-free motion tracing using rf backscatter. In Proceedings of the USENIX Conference on Networked Systems Design and Implementation, NSDI, 2015. 3
[24] M. Kotaru, K. Joshi, D. Bharadia, and S. Katti. Spotfi: Decimeter level localization using wifi. In ACM SIGCOMM Computer Communication Review, 2015. 2
[25] T.-Y. Lin, M. Maire, S. Belongie, J. Hays, P. Perona, D. Ramanan, P. Dollar, and C. L. Zitnick. Microsoft COCO: Com- ´mon objects in context. In Proceedings of the European conference on computer vision, ECCV, 2014. 3, 5
[26] P. Melgarejo, X. Zhang, P. Ramanathan, and D. Chu. Leveraging directional antenna capabilities for fine-grained gesture recognition. In Proceedings of the 2014 ACM International Joint Conference on Pervasive and Ubiquitous Computing, 2014. 3
[27] A. Newell and J. Deng. Associative embedding: End-toend learning for joint detection and grouping. arXiv preprint arXiv:1611.05424, 2016. 1
[28] A. Newell, K. Yang, and J. Deng. Stacked hourglass networks for human pose estimation. In European Conference on Computer Vision, ECCV, 2016. 1
[29] G. Papandreou, T. Zhu, N. Kanazawa, A. Toshev, J. Tompson, C. Bregler, and K. Murphy. Towards Accurate Multiperson Pose Estimation in the Wild. In CVPR, 2017. 2, 5
[30] T. Pfister, J. Charles, and A. Zisserman. Flowing convnets for human pose estimation in videos. In Proceedings of the IEEE International Conference on Computer Vision, ICCV, 2015. 1
[31] L. Pishchulin, E. Insafutdinov, S. Tang, B. Andres, M. Andriluka, P. V. Gehler, and B. Schiele. Deepcut: Joint subset partition and labeling for multi person pose estimation.
In Proceedings of the IEEE Conference on Computer Vision and Pattern Recognition, CVPR, 2016. 1, 2
[32] Q. Pu, S. Gupta, S. Gollakota, and S. Patel. Whole-home gesture recognition using wireless signals. In Proceedings of the 19th Annual International Conference on Mobile computing & Networking, MobiCom. ACM, 2013. 3
[33] M. R. Ronchi and P. Perona. Benchmarking and error diagnosis in multi-instance pose estimation. 2017. 1
[34] A. Salvador, N. Hynes, Y. Aytar, J. Marin, F. Ofli, I. Weber, and A. Torralba. Learning cross-modal embeddings for cooking recipes and food images. Training, 720:619–508, 2016. 2
[35] L. Sigal, A. O. Balan, and M. J. Black. Humaneva: Synchronized video and motion capture dataset and baseline algorithm for evaluation of articulated human motion. International journal of computer vision, 87(1):4–27, March 2010. 2
[36] R. Socher, M. Ganjoo, C. D. Manning, and A. Ng. Zero-shot learning through cross-modal transfer. In Advances in neural information processing systems, pages 935–943, 2013. 2
[37] J. Song, L. Wang, L. Van Gool, and O. Hilliges. Thin-slicing network: A deep structured model for pose estimation in videos. In The IEEE Conference on Computer Vision and Pattern Recognition (CVPR), 2017. 1
[38] J. T. Springenberg, A. Dosovitskiy, T. Brox, and M. Riedmiller. Striving for simplicity: The all convolutional net. arXiv preprint arXiv:1412.6806, 2014. 6
[39] D. Tran, L. Bourdev, R. Fergus, L. Torresani, and M. Paluri. Learning spatiotemporal features with 3D convolutional networks. In Proceedings of the IEEE international conference on computer vision, ICCV, 2015. 4 
[40] D. Vasisht, S. Kumar, and D. Katabi. Decimeter-level localization with a single wifi access point. In NSDI, 2016. 2
[41] C. Vondrick, H. Pirsiavash, and A. Torralba. Generating videos with scene dynamics. In Advances In Neural Information Processing Systems, NIPS, 2016. 4
[42] L. Wang, Y. Xiong, Z. Wang, Y. Qiao, D. Lin, X. Tang, and L. Van Gool. Temporal segment networks: Towards good practices for deep action recognition. In ECCV, 2016. 4
[43] W. Wang, A. X. Liu, and M. Shahzad. Gait recognition using WiFi signals. In Proceedings of the 2016 ACM International Joint Conference on Pervasive and Ubiquitous Computing. ACM, 2016. 3
[44] J. Xiong and K. Jamieson. Arraytrack: A fine-grained indoor location system. In Proceedings of the USENIX Conference on Networked Systems Design and Implementation, NSDI, 2013. 2
[45] S. Yan, Y. Xiong, and D. Lin. Spatial temporal graph convolutional networks for skeleton-based action recognition. In AAAI, 2018. 1
[46] W. Yang, W. Ouyang, H. Li, and X. Wang. End-to-end learning of deformable mixture of parts and deep convolutional neural networks for human pose estimation. In CVPR, 2016. 1
[47] W. Yang, W. Ouyang, X. Wang, J. Ren, H. Li, and X. Wang. 3d human pose estimation in the wild by adversarial learning. In Proceedings of the IEEE Conference on Computer Vision and Pattern Recognition, 2018. 1
[48] M. D. Zeiler, D. Krishnan, G. W. Taylor, and R. Fergus. Deconvolutional networks. In Proceedings of the IEEE Conference on Computer Vision and Pattern Recognition, CVPR, 2017. 4
[49] Y. Zeng, P. H. Pathak, and P. Mohapatra. Wiwho: wifi-based person identification in smart spaces. In Proceedings of the 15th International Conference on Information Processing in Sensor Networks, page 4, 2016. 3
[50] Z. Zhang. Microsoft kinect sensor and its effect. IEEE multimedia, 19(2):4–10, 2012. 2
[51] M. Zhao, F. Adib, and D. Katabi. Emotion recognition using wireless signals. In Proceedings of the 22nd Annual International Conference on Mobile Computing and Networking, MobiCom, 2016. 3
[52] M. Zhao, S. Yue, D. Katabi, T. S. Jaakkola, and M. T. Bianchi. Learning sleep stages from radio signals: A conditional adversarial architecture. In International Conference on Machine Learning, ICML, 2017. 3
