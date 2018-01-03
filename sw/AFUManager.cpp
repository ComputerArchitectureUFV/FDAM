//
// Created by lucas on 12/28/17.
//

#include <assert.h>
#include <cmath>
#include "AFUManager.h"


AFUManager::AFUManager(const char *accel_uuid) {

    AFUManager::fpga = new OPAE_SVC_WRAPPER(accel_uuid);
    assert(AFUManager::fpga->isOk());
    AFUManager::csrs = new CSR_MGR(*AFUManager::fpga);
    AFUManager::readInfoHwAfu();
    assert(AFUManager::numAFUs > 0);
    AFUManager::createWorkspace();
    AFUManager::createAFUs();
}

AFUManager::~AFUManager() {
    AFUManager::clear();
}

void AFUManager::readInfoHwAfu() {
    uint64_t inf[8];
    inf[0] = AFUManager::readCSR(INF_1);
    inf[1] = AFUManager::readCSR(INF_2);
    inf[2] = AFUManager::readCSR(INF_3);
    inf[3] = AFUManager::readCSR(INF_4);
    inf[4] = AFUManager::readCSR(INF_5);
    inf[5] = AFUManager::readCSR(INF_6);
    inf[6] = AFUManager::readCSR(INF_7);
    inf[7] = AFUManager::readCSR(INF_8);
    int index = 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            AFUManager::afuInfo[index] = (int) ((inf[i] >> j * 8) & 0xFF);
            index++;
        }
    }
    AFUManager::numAFUs = 0;
    AFUManager::numInputBuffers = 0;
    AFUManager::numOutputBuffers = 0;
    for (int k = 0; k < AFU_INF_SIZE; k += 2) {
        if (AFUManager::afuInfo[k] != 0 && AFUManager::afuInfo[k + 1] != 0) {
            AFUManager::numAFUs++;
        }
        AFUManager::numInputBuffers += AFUManager::afuInfo[k];
        AFUManager::numOutputBuffers += AFUManager::afuInfo[k + 1];
    }

}

AFU *AFUManager::getAFU(afu_id id) {
    if (id < AFUManager::numAFUs) {
        return AFUManager::AFUs.at(id);
    } else {
        BEGIN_COLOR(RED);
        MSG("AFU ID invalid!");
        END_COLOR();
        return nullptr;
    }
}

void AFUManager::writeCSR(uint32_t regID, uint64_t val) {
    AFUManager::csrs->writeCSR(regID, val);
}

uint64_t AFUManager::readCSR(uint32_t regID) {
    return AFUManager::csrs->readCSR(regID);
}

bool AFUManager::AFUIsSimulated() {
    return AFUManager::fpga->hwIsSimulated();
}

void *AFUManager::fpgaAllocBuffer(size_t numBytes) {
    AFUManager::commitedWorkspace = false;
    return AFUManager::fpga->allocBuffer(numBytes);
}

void AFUManager::fpgaFreeBuffer(void *ptr) {
    AFUManager::commitedWorkspace = false;
    AFUManager::fpga->freeBuffer(ptr);
}

void AFUManager::createWorkspace() {
    int num_cl_conf_in = (int) (2 * std::ceil((double)AFUManager::numInputBuffers / 8));
    int num_cl_conf_out = (int) (2 * std::ceil((double)AFUManager::numOutputBuffers / 8));
    AFUManager::numClConfs = num_cl_conf_in + num_cl_conf_out;
    AFUManager::numClDSM = 1 + (AFUManager::numClConfs / 2);
    size_t workspaceSize = (size_t) CL(AFUManager::numClConfs + AFUManager::numClDSM);
    AFUManager::workspace = (uint64_t *) AFUManager::fpgaAllocBuffer(workspaceSize);
    assert(NULL != AFUManager::workspace);
    AFUManager::dsm = (uint64_t *) (((char *) AFUManager::workspace) + CL(AFUManager::numClConfs));
    memset(AFUManager::workspace, 0x00, workspaceSize);
    AFUManager::writeCSR(ADDR_WORKSPACE_BASE, (uint64_t) intptr_t(AFUManager::workspace));
    uint64_t size = (uint64_t) AFUManager::numClDSM;
    size = size << 16;
    size |= AFUManager::numClConfs;
    AFUManager::writeCSR(WORKSPACE_SIZE, size);
}

void AFUManager::createAFUs() {
    for (int i = 0; i < AFUManager::numAFUs; ++i) {
        int numInputBuffers = afuInfo[i * 2];
        int numOutputBuffers = afuInfo[i * 2 + 1];
        afu_id id = (afu_id) i;
        AFUManager::AFUs[id] = new AFU(*this, id, numInputBuffers, numOutputBuffers);
    }
}

void AFUManager::clear() {
    AFUManager::commitedWorkspace = false;
    AFUManager::writeCSR(CFG_REG, AFU_CONTROLLER_STOP);
    for (map<afu_id, AFU *>::iterator it = AFUManager::AFUs.begin(); it != AFUManager::AFUs.end(); ++it)
        delete (it->second);
    AFUManager::fpgaFreeBuffer(AFUManager::workspace);
    delete AFUManager::csrs;
    delete AFUManager::fpga;
}

void AFUManager::commitWorkspace() {
    AFUManager::commitedWorkspace = false;
    AFUManager::writeCSR(CFG_REG, AFU_CONTROLLER_STOP);
    AFUManager::writeCSR(CFG_REG, AFU_CONTROLLER_UPDATE_WKP);
    int result = 1;
    struct timespec pause;
    pause.tv_sec = 1;
    pause.tv_nsec = 0;
    while (result != 0) {
        nanosleep(&pause, NULL);
        result = (int) (AFUManager::readCSR(AFU_CONTROLLER_STATUS) & 1L);
    };
    AFUManager::writeCSR(CFG_REG, AFU_CONTROLLER_START);
    while (result == 0) {
        nanosleep(&pause, NULL);
        result = (int) (AFUManager::readCSR(AFU_CONTROLLER_STATUS) & 1L);
    };
    AFUManager::commitedWorkspace = true;
}

bool AFUManager::workspaceIscommited() {
    return AFUManager::commitedWorkspace;
}
void AFUManager::printStatics() {
    double total_cicles = AFUManager::readCSR(CLOCK_COUNT);
    double total_cl_wr = AFUManager::readCSR(CL_WR_COUNT);
    double total_cl_rd = AFUManager::readCSR(CL_RD_COUNT);
    double totalBytesBuffers = CL(total_cl_wr + total_cl_rd);
    double cicle_time = 1.0 / (AFUManager::csrs->getAFUMHz() * 1000000.0);
    double time_spend = cicle_time * total_cicles;
    double speed = 0;
    if (time_spend != 0) {
        speed = totalBytesBuffers / time_spend;
        speed = speed / GB(1);
    }
    // Reads CSRs to get some statistics
    BEGIN_COLOR(GREEN);
    MSG("Total Clock cicles: " << std::dec << total_cicles);
    MSG("AFUManager frequency: " << std::dec << AFUManager::csrs->getAFUMHz() << " MHz");
    MSG("AFUManager TARGET:" << (AFUManager::fpga->hwIsSimulated() ? " [simulated]" : "[real device]"));
    MSG("Transfer speed:" << speed << " GB/s");

    // MFP VTP (virtual to physical) statistics
    if (mpfVtpIsAvailable(AFUManager::fpga->mpf_handle)) {
        mpf_vtp_stats vtp_stats;
        mpfVtpGetStats(AFUManager::fpga->mpf_handle, &vtp_stats);
        if (vtp_stats.numFailedTranslations) {
            MSG("VTP failed translating VA: 0x" << hex << uint64_t(vtp_stats.ptWalkLastVAddr) << dec);
        }
        MSG("VTP PT walk cycles: " << vtp_stats.numPTWalkBusyCycles);
        MSG("VTP L2 4KB hit / miss: " << vtp_stats.numTLBHits4KB << " / " << vtp_stats.numTLBMisses4KB);
        MSG("VTP L2 2MB hit / miss: " << vtp_stats.numTLBHits2MB << " / " << vtp_stats.numTLBMisses2MB);
    }
    END_COLOR();
}

void AFUManager::printInfoAFU() {
    BEGIN_COLOR(BLUE);
    MSG("AFUManager INFO:");
    MSG("NUM AFUS: " << numAFUs);
    for (int i = 0; i < numAFUs; ++i) {
        MSG("AFU " << i << ": Input Buffer: " << afuInfo[i] << " Output Buffer: " << afuInfo[i + 1]);
    }
    END_COLOR();
}
void AFUManager::printWorkspace() {
    for (int i = 0; i < AFUManager::numClConfs; ++i) {
        for (int j = 0; j < 8; ++j) {
          cout << std::hex << AFUManager::workspace[GET_INDEX(i,j,8)] << " ";
        }
        cout << endl;
    }
}

void AFUManager::printDSM() {
    BEGIN_COLOR(GREEN);
    MSG("DSM:");
    for (int i = 0; i < AFUManager::numClDSM; ++i) {
        cout <<"  [APP]  ";
        for (int j = 7; j >= 0; --j) {
            cout << std::hex << AFUManager::dsm[GET_INDEX(i,j,8)] << " ";
        }
        cout << endl;
    }
    END_COLOR();
}

int AFUManager::getNumClConf() {
    return AFUManager::numClConfs;
}

int AFUManager::getNumClDSM() {
    return AFUManager::numClDSM;
}

