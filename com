mmcinit
fatload mmc 0 a3000000 kernel.bin
fatload mmc 0 a0000000 package.bin
go a3000000 2345
