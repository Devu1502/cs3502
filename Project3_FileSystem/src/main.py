import tkinter as tk

def main():
    root = tk.Tk()
    root.title("File Manager")
    root.geometry("600x400")

    label = tk.Label(root, text="My File System", font=("Arial", 16))
    label.pack(pady=10)

    root.mainloop()

if __name__ == "__main__":
    main()
    