import tkinter as tk
from tkinter import messagebox
from file_ops import list_files, read_file, create_file, update_file, delete_file, rename_file, create_directory
import os
from datetime import datetime

def main():
    root = tk.Tk()
    root.title("File Manager")
    root.geometry("600x400")
    selected_file = None

    current_path = os.getcwd()

    label = tk.Label(root, text=f"Path: {current_path}", font=("Arial", 10))
    label.pack(pady=5)

    entry = tk.Entry(root, width=50)
    entry.pack(pady=5)

    listbox = tk.Listbox(root, width=80)
    listbox.pack(pady=10)
    text_area = tk.Text(root, height=10, width=80)
    text_area.pack(pady=10)
    status_label = tk.Label(root, text="Ready", fg="green")
    status_label.pack(pady=5)

    files = list_files(current_path)

    for file in files:
        listbox.insert(tk.END, file)

    def show_metadata(path):
        stats = os.stat(path)
        size = stats.st_size
        modified = datetime.fromtimestamp(stats.st_mtime)

        messagebox.showinfo("Metadata", f"Size: {size} bytes\nModified: {modified}")


    def refresh_list(path):
        listbox.delete(0, tk.END)
        files = list_files(path)
        for file in files:
            listbox.insert(tk.END, file)

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
            status_label.config(text="File created successfully")
        else:
            messagebox.showerror("Error", "Failed to create file")
    
    create_button = tk.Button(root, text="Create File", command=create_file_ui)
    create_button.pack(pady=5)

    def create_directory_ui():
        dirname = entry.get()

        if not dirname:
            messagebox.showerror("Error", "Directory name cannot be empty")
            return

        full_path = os.path.join(current_path, dirname)

        success = create_directory(full_path)

        if success:
            listbox.insert(tk.END, dirname)
            entry.delete(0, tk.END)
            status_label.config(text="Directory created")
        else:
            messagebox.showerror("Error", "Failed to create directory")
        
    dir_button = tk.Button(root, text="Create Directory", command=create_directory_ui)
    dir_button.pack(pady=5)

    def save_file():
        if not selected_file:
            messagebox.showerror("Error", "No file selected")
            return

        content = text_area.get("1.0", tk.END)
        success = update_file(selected_file, content)

        if success:
            messagebox.showinfo("Success", "File saved successfully")
            status_label.config(text="File saved")
        else:
            messagebox.showerror("Error", "Failed to save file")

    save_button = tk.Button(root, text="Save File", command=save_file)
    save_button.pack(pady=5)

    def delete_file_ui():
        nonlocal selected_file

        if not selected_file:
            return

        confirm = messagebox.askyesno("Confirm Delete", "Are you sure you want to delete this file?")

        if confirm:
            success = delete_file(selected_file)

            if success:
                selected_index = listbox.curselection()
                if selected_index:
                    listbox.delete(selected_index)
                text_area.delete("1.0", tk.END)
                selected_file = None
                status_label.config(text="File deleted")

            if not success:
                messagebox.showerror("Error", "Failed to delete file")

    delete_button = tk.Button(root, text="Delete File", command=delete_file_ui)
    delete_button.pack(pady=5)

    def rename_file_ui():
        nonlocal selected_file

        if not selected_file:
            messagebox.showerror("Error", "No file selected")
            return

        new_name = entry.get()

        if not new_name:
            messagebox.showerror("Error", "Enter new name")
            return

        new_path = os.path.join(current_path, new_name)

        success = rename_file(selected_file, new_path)

        if success:
            listbox.delete(tk.ANCHOR)
            listbox.insert(tk.END, new_name)
            selected_file = new_path
            entry.delete(0, tk.END)
            status_label.config(text="File renamed")
        else:
            messagebox.showerror("Error", "Rename failed")
    
    rename_button = tk.Button(root, text="Rename File", command=rename_file_ui)
    rename_button.pack(pady=5)

    def on_file_select(event):
        nonlocal selected_file, current_path

        if not listbox.curselection():
            return

        selected = listbox.get(listbox.curselection())
        full_path = os.path.join(current_path, selected)

        if os.path.isdir(full_path):
            current_path = full_path
            label.config(text=f"Path: {current_path}")
            refresh_list(current_path)
            selected_file = None
            text_area.delete("1.0", tk.END)

        elif os.path.isfile(full_path):
            selected_file = full_path
            content = read_file(full_path)
            show_metadata(full_path)

            text_area.delete("1.0", tk.END)
            if content is not None:
                text_area.insert(tk.END, content)

    def go_back():
        nonlocal current_path

        parent = os.path.dirname(current_path)

        if parent != current_path:
            current_path = parent
            label.config(text=f"Path: {current_path}")
            refresh_list(current_path)

    back_button = tk.Button(root, text="Go Back", command=go_back)
    back_button.pack(pady=5)


    listbox.bind("<<ListboxSelect>>", on_file_select)

    root.mainloop()

if __name__ == "__main__":
    main()