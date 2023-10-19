import os
import shutil


def move_files_to_folder(path: str):

    # Extract the file name from the path
    file_name = os.path.basename(path)

    # Remove the file extension from the name
    folder = os.path.splitext(file_name)[0]

    # Create a folder under the same directory as the file
    folder = os.path.join(os.path.dirname(path), folder)

    # Create a folder to store the files
    if not os.path.exists(folder):
        os.mkdir(folder)

    # Create a folder to store the pictures
    pictures_folder = os.path.join(folder, 'jpg')
    if not os.path.exists(pictures_folder):
        os.mkdir(pictures_folder)

    # Move the files to the folder
    for file in os.listdir():
        if file.endswith('.mp4'):
            shutil.move(file, folder)
        if file.endswith('.csv'):
            shutil.move(file, folder)
        if file.endswith('.jpg'):
            shutil.move(file, pictures_folder)
    
    # Move the bin file to the folder
    shutil.move(path, folder)

    # Move the related avi file to the folder
    if os.path.exists(os.path.splitext(path)[0] + '.avi'):
        shutil.move(os.path.splitext(path)[0] + '.avi', folder)

    # Print out the folder name
    print("The files are moved to the folder: {}".format(folder))


def decode_csv_from_bin(bin_file: str):

    # Print out the executing command
    print("Executing command: ./pc_bin2csv {}".format(bin_file))

    # If the system is Windows
    if os.name == 'nt':
        os.system('pc_bin2csv.exe ' + bin_file)
    else:
        os.system('./pc_bin2csv ' + bin_file)

    # Move the files to a folder
    move_files_to_folder(bin_file)
