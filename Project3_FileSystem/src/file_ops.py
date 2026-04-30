import os

def list_files(path):
    try:
        return os.listdir(path)
    except Exception as e:
        print("Error listing files:", e)
        return []

def read_file(path):
    try:
        with open(path, 'r') as f:
            return f.read()
    except Exception as e:
        print("Error reading file:", e)
        return None

def create_file(path, content):
    try:
        with open(path, 'w') as f:
            f.write(content)
        return True
    except Exception as e:
        print("Error creating file:", e)
        return False

def update_file(path, content):
    try:
        with open(path, 'w') as f:
            f.write(content)
        return True
    except Exception as e:
        print("Error updating file:", e)
        return False

def delete_file(path):
    import os
    try:
        os.remove(path)
        return True
    except Exception as e:
        print("Error deleting file:", e)
        return False