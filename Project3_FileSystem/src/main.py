import tkinter as tk
from file_ops import list_files, read_file, create_file, update_file
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
            return

        full_path = os.path.join(current_path, filename)
        success = create_file(full_path, "")

        if success:
            listbox.insert(tk.END, filename)
            entry.delete(0, tk.END)

    button = tk.Button(root, text="Create File", command=create_file_ui)
    button.pack(pady=5)

    def save_file():
        if selected_file:
            content = text_area.get("1.0", tk.END)
            update_file(selected_file, content)

    save_button = tk.Button(root, text="Save File", command=save_file)
    save_button.pack(pady=5)

    listbox.bind("<<ListboxSelect>>", on_file_select)

    root.mainloop()

if __name__ == "__main__":
    main()