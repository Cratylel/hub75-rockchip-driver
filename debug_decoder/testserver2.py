import socket
import threading
import tkinter as tk
from PIL import Image, ImageTk
pins = ["OE", "STB", "CLK", "A", "B", "C", "D", "E", "R1", "G1", "B1", "R2", "G2", "B2"]


row_addr = 0
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
        self.current_x = 63
        self.pin_states = {
            "OE": False,
            "STB": False,
            "CLK": False,
            "A": False,
            "B": False,
            "C": False,
            "D": False,
            "E": False,
            "R1": False,
            "G1": False,
            "B1": False,
            "R2": False,
            "G2": False,
            "B2": False
        }
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

    # def process_command(self, command):
    #     if command.lower() == "clear":
    #         self.clear_matrix()
    #         return

    #     try:
    #         pos_part, color_part = command.split(";")
    #         x, y = map(int, pos_part.split(","))
    #         r, g, b = map(int, color_part.split(","))
    #         if 0 <= x < self.width and 0 <= y < self.height:
    #             self.matrix[y][x] = rgb111_to_rgb888(r, g, b)
    #     except Exception as e:
    #         print(f"Invalid command '{command}': {e}")
    
    def process_command(self, command):
        if command.lower() == "clear":
            self.clear_matrix()
            return

        #try:
        # print(command)
        pin, value = command.split(",")
        pin = pin.strip().upper()
        value = int(value.strip())

        if pin in self.pin_states:
            self.pin_states[pin] = bool(value)
            if (pin == "STB" or pin == "CLK") and value == 1:
                    self.execute_function(pin)
            return
        

    def execute_function(self, pin):
        
        if pin == "STB":
            print("Received row lock request")
            
            self.current_x = 63
        elif pin == "CLK":
           # print(self.pin_states["CLK"])
          #  print(f"X: {self.current_x}, Y: {self.calculate_row()}")
            self.matrix[self.calculate_row()][self.current_x] = rgb111_to_rgb888(
                self.pin_states["R1"], self.pin_states["G1"], self.pin_states["B1"]
            )
            self.matrix[self.calculate_row() + 16][self.current_x] = rgb111_to_rgb888(
                self.pin_states["R2"], self.pin_states["G2"], self.pin_states["B2"]
            )
            # self.matrix[0][0] = (self.pin_states["R1"], self.pin_states["G1"], self.pin_states["B1"])
            # self.matrix[0][16] = (self.pin_states["R2"], self.pin_states["G2"], self.pin_states["B2"])
            #self.matrix[30][10] = (255, 255, 255)
            self.current_x -= 1
        
    def calculate_row(self):

        # Each pin A-E represents a binary bit value; E is the most significant bit
        addr = (
            (self.pin_states["E"] << 4) |
            (self.pin_states["D"] << 3) |
            (self.pin_states["C"] << 2) |
            (self.pin_states["B"] << 1) |
            self.pin_states["A"]
        )
       # print(self.pin_states["A"], self.pin_states["B"], self.pin_states["C"], self.pin_states["D"], self.pin_states["E"])
        #print(f"Address: {addr}")
        return addr
        

    def clear_matrix(self):
        """Reset all pixels to black (0, 0, 0)."""
        self.matrix = [[(0, 0, 0) for _ in range(self.width)] for _ in range(self.height)]
        print("Matrix cleared")

# Start the matrix viewer
RGBMatrixViewer()
# Start the matrix viewer
