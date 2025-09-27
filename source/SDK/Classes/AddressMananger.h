#pragma once
void CheckAddressesLoop() {
    while (true) {
        try {

            Cache::DataModel = dataModel->getAddress();
            Cache::VisualEngine = visualEngine->getAddress();

            u64 fakeDataModel_ = comms->read<u64>(comms->image_address + Offsets::FakeDataModel);
            u64 realDataModelAddr = comms->read<u64>(fakeDataModel_ + Offsets::RealDataModel);
            
            if (realDataModelAddr != Cache::DataModel) {
                if (runningThread) {
                    runningThread = false;
                }
                printf("[*] 주소 변경 감지: 업데이트 중...\n");
                std::unique_ptr<RBX::DataModel> NewDataModel = std::make_unique<RBX::DataModel>(realDataModelAddr);
                
                if (!comms->is_connected()) {
                    printf("[!] 드라이버 연결이 끊어졌습니다. 재연결 시도 중...\n");
                    comms = std::make_unique<communication>();
                    
                    if (!comms->is_connected()) {
                        printf("[!] 드라이버에 재연결 실패. 종료합니다.\n");
                        sleep_ms(5000);
                        exit(0);
                    }
                }
                
                while (comms->find_process("RobloxPlayerBeta.exe") == 0) {
                    printf("[*] Roblox 프로세스 대기 중...\n");
                    sleep_ms(500);
                }
                
                if (!comms->v_attach(comms->process_id)) {
                    printf("[!] 프로세스 연결 실패. 종료합니다.\n");
                    sleep_ms(5000);
                    exit(0);
                }
                
                if (comms->find_image() == 0) {
                    printf("[!] 베이스 주소를 찾을 수 없습니다. 종료합니다.\n");
                    sleep_ms(5000);
                    exit(0);
                }
                                dataModel = std::move(NewDataModel);
                printf("[+] DataModel 주소 업데이트: 0x%llx\n", dataModel->getAddress());
                

                visualEngine = std::make_unique<RBX::VisualEngine>(
                    comms->read<u64>(comms->image_address + Offsets::VisualEngine)
                );
                printf("[+] VisualEngine 주소 업데이트: 0x%llx\n", visualEngine->getAddress());
                
                // 캐싱된 주소 업데이트
                Cache::DataModel = dataModel->getAddress();
                Cache::VisualEngine = visualEngine->getAddress();
                DataModelOld = Cache::DataModel;
                VisualEngineOld = Cache::VisualEngine;
            }
        }
        catch (const std::exception& e) {
            printf("[!] 주소 확인 중 오류 발생: %s\n", e.what());
        }
        
        sleep_ms(100);
    }
}
