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

def create_file_ui():
    filename = entry.get()

    if not filename:
        messagebox.showerror("Error", "Filename cannot be empty")
        return

    full_path = os.path.join(current_path, filename)

    if os.path.exists(full_path):
        messagebox.showerror("Error", "File already exists")
        return

    success = create_file(full_path, "")

    if success:
        listbox.insert(tk.END, filename)
        entry.delete(0, tk.END)
    else:
        messagebox.showerror("Error", "Failed to create file")