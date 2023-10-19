import os
import pandas as pd


def load_images_from_folder(folder_path: str):
    image_dict = {}

    for root, dirs, files in os.walk(folder_path):
        for file in files:
            if file.endswith('jpg') or file.endswith('png'):
                image_dict[file] = os.path.join(root, file)

    return image_dict


def rename_images(image_dict: dict, start_time):

    # Remove .jpg or .png from the image name
    temp_imgs = []
    for image_fn, image_path in image_dict.items():

        # Remove the extension
        fn_timestamp = image_fn.split('.')[0]

        # Convert it to integer 
        fn_timestamp = int(fn_timestamp)

        # Add it to the list
        temp_imgs.append((fn_timestamp, image_path))

    # Convert the tuple of list into DataFrame
    temp_pd = pd.DataFrame(temp_imgs, columns=['Timestamp', 'ImagePath'])

    # Add a column to DataFrame based on CSV timestamp
    temp_pd['Timestamp_CSV'] = start_time + 8

    # Calculate the timestamp gap, Timestamp[n+1] - Timestamp[n]
    temp_pd['Timestamp_GAP'] = temp_pd['Timestamp'].diff()
    temp_pd['Timestamp_GAP'] = temp_pd['Timestamp_GAP'].fillna(0)
    temp_pd['Timestamp_GAP'] = temp_pd['Timestamp_GAP'].astype(int)

    # Calculate the accumulated timestamp gap
    temp_pd['Accumulated_GAP'] = temp_pd['Timestamp_GAP'].cumsum()

    # Calculate new timestamp, Accumulated_GAP + Timestamp_CSV
    temp_pd['Timestamp_New'] = temp_pd['Accumulated_GAP'] + temp_pd['Timestamp_CSV']

    # Based on timestamp_with_dirs, rename the images
    for index, row in temp_pd.iterrows():
        # Get the timestamp and image path
        timestamp = row['Timestamp_New']
        image_path = row['ImagePath']

        # Get the image name
        image_fn = os.path.basename(image_path)

        # Get the image extension
        image_ext = os.path.splitext(image_fn)[1]

        # Get the image directory
        image_dir = os.path.dirname(image_path)

        # Get the new image name
        new_image_fn = str(timestamp) + image_ext

        # Get the new image path
        new_image_path = os.path.join(image_dir, new_image_fn)

        # Rename the image
        os.rename(image_path, new_image_path)


def rename_pics_in_dir(root_dir: str):
    # Scan the directory and get all the image files
    for root, dirs, files in os.walk(root_dir):

        # Load the images
        images = None
        for dir in dirs:
            if dir == 'jpg' or dir == 'png':
                images = load_images_from_folder(os.path.join(root, dir))

        # Load the CSV file
        timestamp_list = None
        for file in files:
            if file.endswith('csi_cleaned.csv'):
                csv_pd = pd.read_csv(os.path.join(root, file), sep='\t', encoding='utf-8')
        
                # Get the timestamp list from the CSV file
                timestamp_list = csv_pd['timestamp'].tolist()

        # Check if the timestamp list is empty
        if timestamp_list is None or images is None:
            continue

        # Get the first and last timestamp
        first_timestamp = int(timestamp_list[0])
        last_timestamp = int(timestamp_list[-1])

        # Rename the images
        rename_images(images, first_timestamp)
