import socket
import threading
import tkinter as tk
from PIL import Image, ImageTk

# RGB111 to RGB888
def rgb111_to_rgb888(r, g, b):
    return (
        255 if r else 0,
        255 if g else 0,
        255 if b else 0
    )

class RGBMatrixViewer:
    def __init__(self, width=64, height=32, scale=10):
        self.width = width
        self.height = height
        self.scale = scale
        self.matrix = [[(0, 0, 0) for _ in range(width)] for _ in range(height)]

        self.root = tk.Tk()
        self.root.title("RGB111 Matrix")

        self.img = Image.new("RGB", (width, height))
        self.tk_img = ImageTk.PhotoImage(self.img.resize((width * scale, height * scale), Image.NEAREST))
        self.label = tk.Label(self.root, image=self.tk_img)
        self.label.pack()

        self.update_image()
        self.root.after(100, self.update_image)
        threading.Thread(target=self.start_server, daemon=True).start()
        self.root.mainloop()

    def update_image(self):
        for y in range(self.height):
            for x in range(self.width):
                self.img.putpixel((x, y), self.matrix[y][x])

        scaled = self.img.resize((self.width * self.scale, self.height * self.scale), Image.NEAREST)
        self.tk_img = ImageTk.PhotoImage(scaled)
        self.label.config(image=self.tk_img)
        self.label.image = self.tk_img
        self.root.after(100, self.update_image)

    def start_server(self, host='0.0.0.0', port=5000):
        print(f"Starting socket server on {host}:{port}")
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.bind((host, port))
            s.listen()
            while True:
                conn, addr = s.accept()
                print(f"Connected by {addr}")
                threading.Thread(target=self.handle_client, args=(conn,), daemon=True).start()

    def handle_client(self, conn):
        with conn:
            buffer = ""
            while True:
                data = conn.recv(1024)
                if not data:
                    break
                buffer += data.decode()

                # Process full commands line by line
                while "\n" in buffer:
                    line, buffer = buffer.split("\n", 1)
                    self.process_command(line.strip())

    def process_command(self, command):
        if command.lower() == "clear":
            self.clear_matrix()
            return

        try:
            pos_part, color_part = command.split(";")
            x, y = map(int, pos_part.split(","))
            r, g, b = map(int, color_part.split(","))
            if 0 <= x < self.width and 0 <= y < self.height:
                self.matrix[y][x] = rgb111_to_rgb888(r, g, b)
        except Exception as e:
            print(f"Invalid command '{command}': {e}")

    def clear_matrix(self):
        """Reset all pixels to black (0, 0, 0)."""
        self.matrix = [[(0, 0, 0) for _ in range(self.width)] for _ in range(self.height)]
        print("Matrix cleared")

# Start the matrix viewer
RGBMatrixViewer()
