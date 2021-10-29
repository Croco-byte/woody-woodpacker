# woody-woodpacker
Woody-woodpacker project from 42 (Crypter)

Woody-woopacker is a packer that encrypts an ELF64 executable (this type of packer is known as a Crypter). After encrypting, it will inject in the file a small piece of executable code that will allow the binary to decrypt itself before executing. This executable code will be injected at the end of the text segment. ELF64 segments are aligned to page length (4096 bytes) ; which means that there often will be some padding at the end of the segment. To maximize stealth and efficiency, if the padding is big enough for our payload, we will use this space to inject the payload. If it isn't big enough, we will extend the text segment of the original file in order to fit the payload.

This crypter can also encrypt PIC (Position Independent Code), by leaking at runtime the address space of the program and deducing the rest with offsets. The leak is triggered by pushing RIP on the stack, and popping it in a general purpose register.

