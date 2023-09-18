from ultralytics import YOLO
import csv
import cv2
import os
import sys


def find_image(path: str):
    # If the folder path is not valid, exit the program
    if not os.path.isdir(path):
        print("The folder path is not valid!")
        sys.exit(1)

    # 加载当前目录下的所有图片
    image_list = []
    for f_path in os.listdir(path):
        if os.path.isfile(os.path.join(path, f_path)):
            if f_path.endswith('.png'):
                image_list.append(os.path.join(path, f_path))
            if f_path.endswith('.jpg'):
                image_list.append(os.path.join(path, f_path))

    return image_list


def detect_human_save_bx_to_csv(model, image_list):

    # Run batched inference on a list of images
    results = model(image_list)  # return a list of Results objects

    # create a csv file
    with open('result.csv', 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(['image_fn', 'x1', 'y1', 'x2', 'y2'])

        # 对于每张图片的结果
        for img_idx, result in enumerate(results):

            # 把数据移动到CPU上
            boxes = result.boxes.cpu().numpy()
            xyxys = boxes.xyxy

            # 对于每个检测到的物体
            for idx, xyxy in enumerate(xyxys):

                # draw the bounding box
                if boxes.cls[idx] == 0:
                    xyxy0 = int(xyxy[0])
                    xyxy1 = int(xyxy[1])
                    xyxy2 = int(xyxy[2])
                    xyxy3 = int(xyxy[3])

                    # save the bounding box to csv file
                    writer.writerow([image_list[img_idx], xyxy0, xyxy1, xyxy2, xyxy3])

    

if __name__ == '__main__':
    # Load a model
    model = YOLO('yolov8n.pt')  # pretrained YOLOv8n model

    # Find the image
    image_list = find_image(sys.argv[1])

    # Detect the human and save the bounding box to csv file
    detect_human_save_bx_to_csv(model, image_list)