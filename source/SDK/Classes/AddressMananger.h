#pragma once
void CheckAddressesLoop() {
    while (true) {
        try {
            // 현재 주소 캐싱
            Cache::DataModel = dataModel->getAddress();
            Cache::VisualEngine = visualEngine->getAddress();
            
            // FakeDataModel을 통해 실제 DataModel 주소 확인
            u64 fakeDataModel_ = comms->read<u64>(comms->image_address + Offsets::FakeDataModel);
            u64 realDataModelAddr = comms->read<u64>(fakeDataModel_ + Offsets::RealDataModel);
            
            // DataModel 주소가 변경되었는지 확인
            if (realDataModelAddr != Cache::DataModel) {
                if (runningThread) {
                    runningThread = false;
                }
                printf("[*] 주소 변경 감지: 업데이트 중...\n");
                
                // 새로운 DataModel 생성
                std::unique_ptr<RBX::DataModel> NewDataModel = std::make_unique<RBX::DataModel>(realDataModelAddr);
                
                // 기존 연결이 여전히 유효한지 확인
                if (!comms->is_connected()) {
                    printf("[!] 드라이버 연결이 끊어졌습니다. 재연결 시도 중...\n");
                    comms = std::make_unique<communication>();
                    
                    if (!comms->is_connected()) {
                        printf("[!] 드라이버에 재연결 실패. 종료합니다.\n");
                        sleep_ms(5000);
                        exit(0);
                    }
                }
                
                // 프로세스 재확인 및 연결
                while (comms->find_process("RobloxPlayerBeta.exe") == 0) {
                    printf("[*] Roblox 프로세스 대기 중...\n");
                    sleep_ms(500);
                }
                
                if (!comms->v_attach(comms->process_id)) {
                    printf("[!] 프로세스 연결 실패. 종료합니다.\n");
                    sleep_ms(5000);
                    exit(0);
                }
                
                // 이미지 주소 재확인
                if (comms->find_image() == 0) {
                    printf("[!] 베이스 주소를 찾을 수 없습니다. 종료합니다.\n");
                    sleep_ms(5000);
                    exit(0);
                }
                
                // DataModel 업데이트
                dataModel = std::move(NewDataModel);
                printf("[+] DataModel 주소 업데이트: 0x%llx\n", dataModel->getAddress());
                
                // VisualEngine 업데이트
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
