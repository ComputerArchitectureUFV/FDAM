
#include <cgra/cgra.h>

Cgra::Cgra(unsigned num_pe, unsigned int num_pe_io, unsigned int net_radix) {
    Cgra::cgra_program = nullptr;
    Cgra::accManagement = new AccManagement();
    Cgra::num_pe = num_pe;
    Cgra::num_pe_io = num_pe_io;
    Cgra::net_radix = net_radix;

    auto bits_conf_cross = static_cast<int>(ceil(log2(net_radix) * net_radix));
    auto num_stagies = static_cast<int>(ceil(intlog(num_pe * 2, net_radix)));
    auto num_cross_stagies = static_cast<int>(ceil((num_pe * 2) / net_radix));
    int bits_conf = num_stagies * num_cross_stagies * bits_conf_cross;

    Cgra::net_conf_bytes = static_cast<unsigned int>(ceil(bits_conf / 8));

}

Cgra::~Cgra() {
    Cgra::cgra_program = nullptr;
    delete Cgra::accManagement;
}

void Cgra::prepareProgram(cgra_program_t *program) {

    Cgra::cgra_program = program;
    Accelerator &acc = Cgra::accManagement->getAccelerator(Cgra::cgra_program->cgra_id);

    auto mask_in_queue = Cgra::cgra_program->cgra_intial_conf.read_fifo_mask;
    auto mask_out_queue = Cgra::cgra_program->cgra_intial_conf.write_fifo_mask;
    auto num_bytes = Cgra::cgra_program->input_queues[0].length * sizeof(unsigned short);

    size_t cgra_intial_conf_bytes = sizeof(cgra_intial_conf_t);
    size_t pe_initial_conf_bytes = Cgra::cgra_program->cgra_intial_conf.qtd_pe_init_conf * sizeof(pe_initial_conf_t);
    size_t pe_conf_bytes = Cgra::cgra_program->cgra_intial_conf.qtd_pe_conf * sizeof(pe_conf_t);
    size_t net_conf_total_bytes = Cgra::cgra_program->cgra_intial_conf.qtd_net_conf * Cgra::net_conf_bytes;

    auto cgra_intial_conf_bytes_align = static_cast<int>((std::ceil(cgra_intial_conf_bytes / 64.0)) * 64.0);
    auto pe_initial_conf_bytes_align = static_cast<int>((std::ceil(pe_initial_conf_bytes / 64.0)) * 64.0);
    auto pe_conf_bytes_align = static_cast<int>((std::ceil(pe_conf_bytes / 64.0)) * 64.0);
    auto net_conf_bytes_align = static_cast<int>((std::ceil(net_conf_total_bytes / 64.0)) * 64.0);

    long long int total_bytes;
    total_bytes = cgra_intial_conf_bytes_align;
    total_bytes += pe_initial_conf_bytes_align;
    total_bytes += pe_conf_bytes_align;
    total_bytes += net_conf_bytes_align;
    total_bytes += num_bytes;

    acc.createInputQueue(static_cast<unsigned char>(0), total_bytes);
    auto queue_data_ptr = (unsigned char *) acc.getInputQueue(0);

    memcpy(queue_data_ptr, &Cgra::cgra_program->cgra_intial_conf, cgra_intial_conf_bytes);
    queue_data_ptr = queue_data_ptr + cgra_intial_conf_bytes_align;
    memcpy(queue_data_ptr, Cgra::cgra_program->pe_initial_conf, pe_initial_conf_bytes);
    queue_data_ptr = queue_data_ptr + pe_initial_conf_bytes_align;
    memcpy(queue_data_ptr, Cgra::cgra_program->pe_conf, pe_conf_bytes);
    queue_data_ptr = queue_data_ptr + pe_conf_bytes_align;
    int n = Cgra::cgra_program->cgra_intial_conf.qtd_net_conf;
    for (int j = 0; j < n; ++j) {
        memcpy(queue_data_ptr + (j * Cgra::net_conf_bytes), Cgra::cgra_program->net_conf[j].swicth_conf,
               Cgra::net_conf_bytes);
    }
    queue_data_ptr = queue_data_ptr + net_conf_bytes_align;
    memcpy(queue_data_ptr, Cgra::cgra_program->input_queues[0].data, num_bytes);

    for (int i = 1; i < Cgra::num_pe_io; ++i) {
        if (mask_in_queue & (1 << i)) {
            num_bytes = Cgra::cgra_program->input_queues[i].length * sizeof(short);
            auto id_queue = static_cast<unsigned char>(i);
            acc.createInputQueue(id_queue, num_bytes);
            acc.copyToInputQueue(id_queue, Cgra::cgra_program->input_queues[i].data,
                                 Cgra::cgra_program->input_queues[i].length);
        }
    }
    for (int i = 0; i < Cgra::num_pe_io; ++i) {
        if (mask_out_queue & (1 << i)) {
            auto id_queue = static_cast<unsigned char>(i);
            num_bytes = Cgra::cgra_program->output_queues[i].length * sizeof(short);
            acc.createOutputQueue(id_queue, num_bytes);
        }
    }
}

void Cgra::syncExecute(long waitTime) {
    Accelerator &acc = Cgra::accManagement->getAccelerator((Cgra::cgra_program->cgra_id));
    acc.start();
    acc.waitDone(waitTime);
    for (int i = 0; i < Cgra::num_pe_io; ++i) {
        if (Cgra::cgra_program->cgra_intial_conf.write_fifo_mask & (1 << i)) {
            auto id_queue = static_cast<unsigned char>(i);
            acc.copyFromOutputQueue(id_queue, Cgra::cgra_program->output_queues[i].data,
                                    Cgra::cgra_program->output_queues[i].length);
        }
    }

}

void Cgra::asyncExecute() {
    Accelerator &acc = Cgra::accManagement->getAccelerator((Cgra::cgra_program->cgra_id));
    acc.start();
}

void Cgra::waitExecute(long waitTime) {
    Accelerator &acc = Cgra::accManagement->getAccelerator((Cgra::cgra_program->cgra_id));
    acc.waitDone(waitTime);
    for (int i = 0; i < Cgra::num_pe_io; ++i) {
        if (Cgra::cgra_program->cgra_intial_conf.write_fifo_mask & (1 << i)) {
            auto id_queue = static_cast<unsigned char>(i);
            acc.copyFromOutputQueue(id_queue, Cgra::cgra_program->output_queues[i].data,
                                    Cgra::cgra_program->output_queues[i].length);
        }
    }
}

int Cgra::intlog(double x, double base) {
    return (int) std::ceil(log(x) / log(base));
}

