## Keyboard Programming

Two IC are interacted when programming with keyboard. 1. Keyboard Encoder 2. Keyboard Controller. 

Keyboard Controller directly with the keryboard. When a event occurs in the keyboard, the keyboard controller handle it and send event to the encoder. There are internal buffer inside the keyboard encoder to store data and command received from or sent to the keyboard controller.


Also the Port I/O Register of the keyboard Encoder is below
```bash 
Keyboard Encoder
Port    R/W     Desc
0x60     R      Read Input Buffer
0x60     W      Send Command

Onboard Keyboard Controller 
Port    R/W     Desc
0x64     R      Status Register 
0x64     W      Send Command
```


One point to note most of the time we wait the keyboard controller to be ready for the next command (test bit 0). Otherwise the previous command will be discarded and the new one will start executing, which is undesirable.


Another point to clarify that port 0x60 and 0x64 are the IO port for keyboard controller. We could send command to configure both keyboard encoder and keyboard controller through these ports. If the command is for keyboard encoder, it is sent to encoder in the keyboard by the keyboard controller in our motherboard 





