import os

def list_files(path):
    try:
        return os.listdir(path)
    except Exception as e:
        print("Error listing files:", e)
        return []