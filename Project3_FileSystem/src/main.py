import tkinter as tk
from file_ops import list_files
import os

def main():
    root = tk.Tk()
    root.title("File Manager")
    root.geometry("600x400")

    current_path = os.getcwd()

    label = tk.Label(root, text=f"Path: {current_path}", font=("Arial", 10))
    label.pack(pady=5)

    listbox = tk.Listbox(root, width=80)
    listbox.pack(pady=10)

    files = list_files(current_path)

    for file in files:
        listbox.insert(tk.END, file)

    root.mainloop()

if __name__ == "__main__":
    main()