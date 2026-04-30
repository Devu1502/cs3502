import tkinter as tk
from tkinter import messagebox
from file_ops import list_files, read_file, create_file, update_file, delete_file
import os

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

    files = list_files(current_path)

    for file in files:
        listbox.insert(tk.END, file)

    def on_file_select(event):
        nonlocal selected_file

        if not listbox.curselection():
            return

        selected = listbox.get(listbox.curselection())
        full_path = os.path.join(current_path, selected)

        if os.path.isfile(full_path):
            selected_file = full_path
            content = read_file(full_path)

            text_area.delete("1.0", tk.END)
            if content:
                text_area.insert(tk.END, content)

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
    
    create_button = tk.Button(root, text="Create File", command=create_file_ui)
    create_button.pack(pady=5)

    def save_file():
        if not selected_file:
            messagebox.showerror("Error", "No file selected")
            return

        content = text_area.get("1.0", tk.END)
        success = update_file(selected_file, content)

        if success:
            messagebox.showinfo("Success", "File saved successfully")
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

            if not success:
                messagebox.showerror("Error", "Failed to delete file")

    delete_button = tk.Button(root, text="Delete File", command=delete_file_ui)
    delete_button.pack(pady=5)


    listbox.bind("<<ListboxSelect>>", on_file_select)

    root.mainloop()

if __name__ == "__main__":
    main()