from hw.buffer_controller_generator.make_buffer_controller import make_buffer_controller

cache_data_width = 512
fifo_depth = 5

# (x,y) = x -> quantidade de entradas da interface
#        y -> quantidade de saídas da interface
interfaces = [(2, 1)]

make_buffer_controller(cache_data_width, fifo_depth, interfaces).to_verilog('../hw/gen')
