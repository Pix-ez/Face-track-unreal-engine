# import socket

# def create_socket():
#     sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#     return sock

# def bind_socket(sock, address):
#     try:
#         sock.bind(address)
#         print("Socket ready!")
#     except socket.error as e:
#         print("Issue binding socket!", e)
#         return False
#     return True

# def send_udp_data(sock, address, data):
#     sock.sendto(data.encode(), address)

# if __name__ == "__main__":
#     import cv2
#     import mediapipe as mp

#     mp_face_detection = mp.solutions.face_detection
#     mp_drawing = mp.solutions.drawing_utils

#     # For webcam input:
#     cap = cv2.VideoCapture('http://192.168.0.105:4747/video')

#     server_address = ('127.0.0.1', 54001)
#     sock = create_socket()
#     bind_socket(sock, server_address)

#     with mp_face_detection.FaceDetection(model_selection=0, min_detection_confidence=0.5) as face_detection:
#         while cap.isOpened():
#             success, image = cap.read()
#             if not success:
#                 print("Ignoring empty camera frame.")
#                 # If loading a video, use 'break' instead of 'continue'.
#                 continue

#             # To improve performance, optionally mark the image as not writeable to pass by reference.
#             image.flags.writeable = False
#             image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
#             results = face_detection.process(image)

#             # Draw the face detection annotations on the image.
#             image.flags.writeable = True
#             image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
#             if results.detections:
#                 for detection in results.detections:
#                     mp_drawing.draw_detection(image, detection)
#                     bbox = detection.location_data.relative_bounding_box
#                     h, w, c = image.shape

#                     center_x = int((bbox.xmin + bbox.width / 2) * w)
#                     center_y = int((bbox.ymin + bbox.height / 2) * h)

#                     center = (center_x, center_y)

#                     # Send the center data
#                     send_udp_data(sock, server_address, str(center))

#             # Flip the image horizontally for a selfie-view display.
#             cv2.imshow('MediaPipe Face Detection', cv2.flip(image, 1))
#             if cv2.waitKey(5) & 0xFF == 27:
#                 break

#     cap.release()
#     sock.close()



# import socket
# import cv2
# import mediapipe as mp
# mp_face_detection = mp.solutions.face_detection
# mp_drawing = mp.solutions.drawing_utils


# class UDPServer:
#     buffer_size = 2 * 1024 * 1024
#     port = 54001

#     def __init__(self):
#         self.in_socket = None

#     def init(self):
#         print("Initializing Winsock...")
#         self.in_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

#         server_hint = ('', self.port)

#         try:
#             self.in_socket.bind(server_hint)
#             print("Socket ready!")
#         except socket.error as e:
#             print("Issue binding socket!", e)
#             return

#         self.listen()

#     def listen(self):
#         print("Listening on port:", self.port)
#         while True:
#             client_data, client_address = self.in_socket.recvfrom(self.buffer_size)

#             client_ip = client_address[0]
#             message = client_data.decode()

#             print("Message received from", client_ip, ":", message)

#             # Send "hello" message to the client
#             self.send(client_address, "hello")

#     def send(self, address, message):
#         self.in_socket.sendto(message.encode(), address)

#     def shutdown(self):
#         self.in_socket.close()

# # Usage
# if __name__ == "__main__":
#     server = UDPServer()
#     server.init()



import cv2
import mediapipe as mp
import socket

import numpy as np

buffer_size = 2 * 1024 * 1024

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, buffer_size)

serverAdressPort = ('127.0.0.1', 54000)

mp_face_detection = mp.solutions.face_detection
mp_drawing = mp.solutions.drawing_utils
# For webcam input:
cap = cv2.VideoCapture('http://192.168.0.105:4747/video')

with mp_face_detection.FaceDetection(model_selection=0, min_detection_confidence=1) as face_detection:
    while cap.isOpened():
        success, image = cap.read()
        if not success:
            print("Ignoring empty camera frame.")
            # If loading a video, use 'break' instead of 'continue'.
            continue
        # To improve performance, optionally mark the image as not writeable to pass by reference.
        image.flags.writeable = False
        image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        results = face_detection.process(image)
        # Draw the face detection annotations on the image.
        image.flags.writeable = True
        image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
        if results.detections:
            for detection in results.detections:
                mp_drawing.draw_detection(image, detection)
                bbox = detection.location_data.relative_bounding_box
                h, w, c = image.shape
                center_y = int((bbox.xmin + bbox.width / 2) * w)
                center_y = np.interp(center_y ,[0,500] ,[50 ,-50])
                center_y = int(center_y)
                
                center_z = int((bbox.ymin + bbox.height / 2) * h)
                center_z = np.interp(center_z ,[0,300] ,[120 ,150])
                center_z = int(center_z)
              
                center = f"{center_y} {center_z}"
                data = str(center).encode('utf-8')

                try:
                    # print(f"Sending data: {data}")
                    sock.sendto(data, serverAdressPort)
                    # Send the center data
                except socket.error as e:
                    print(f"Error sending data: {e}")

        # Flip the image horizontally for a selfie-view display.
        cv2.imshow('MediaPipe Face Detection', cv2.flip(image, 1))
        if cv2.waitKey(5) & 0xFF == 27:
            break
cap.release()
