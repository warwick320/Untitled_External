#pragma once
void CheckAddressesLoop() {
    while (runningThread) {
        Cache::DataModel = dataModel->getAddress();
        Cache::VisualEngine = visualEngine->getAddress();

        u64 fakeDataModel_ = comms->read<u64>(comms->image_address + Offsets::FakeDataModel);
        u64 realDataModelAddr = comms->read<u64>(fakeDataModel_ + Offsets::RealDataModel);

        if (realDataModelAddr != Cache::DataModel) {
            if (runningThread) {
                runningThread = false;
            }
            printf("[*] DataModel Changed Restarting External\n");
            break;
        }
        
        sleep_ms(100);
    }
}
