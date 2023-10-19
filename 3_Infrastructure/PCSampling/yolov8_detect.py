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


def detect_human(model, image):
    # Run batched inference on a list of images
    results = model(image)  

    # Results list
    obj_entity = []

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

                # save the bounding box to the list
                obj_entity.append([image_list[img_idx], xyxy0, xyxy1, xyxy2, xyxy3])
    
    return obj_entity


def detect_human_save_bx_to_csv(model, image_list):

    # count
    count = 0

    # create a csv file
    with open('result.csv', 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(['image_fn', 'x1', 'y1', 'x2', 'y2'])

        # execute the following code for every 10 images
        for i in range(0, len(image_list), 10):

            # detect the human
            results = detect_human(model, image_list[i:i+10])

            # save the bounding box to the csv file
            for result in results:
                writer.writerow(result)
                count += 1
    
    # print the number of the bounding box
    print("The number of the bounding box is: ", count)
    

if __name__ == '__main__':
    # Load a model
    model = YOLO('yolov8n.pt')  # pretrained YOLOv8n model

    # Find the image
    image_list = find_image(sys.argv[1] + "/jpg")

    # Detect the human and save the bounding box to csv file
    detect_human_save_bx_to_csv(model, image_list)

    # Move the generated csv file to the current directory
    os.system('mv result.csv ' + sys.argv[1])