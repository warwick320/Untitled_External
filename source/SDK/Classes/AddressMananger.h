#pragma once
void CheckAddressesLoop() {
    while (true) {
        try {
            // ���� �ּ� ĳ��
            Cache::DataModel = dataModel->getAddress();
            Cache::VisualEngine = visualEngine->getAddress();
            
            // FakeDataModel�� ���� ���� DataModel �ּ� Ȯ��
            u64 fakeDataModel_ = comms->read<u64>(comms->image_address + Offsets::FakeDataModel);
            u64 realDataModelAddr = comms->read<u64>(fakeDataModel_ + Offsets::RealDataModel);
            
            // DataModel �ּҰ� ����Ǿ����� Ȯ��
            if (realDataModelAddr != Cache::DataModel) {
                if (runningThread) {
                    runningThread = false;
                }
                printf("[*] �ּ� ���� ����: ������Ʈ ��...\n");
                
                // ���ο� DataModel ����
                std::unique_ptr<RBX::DataModel> NewDataModel = std::make_unique<RBX::DataModel>(realDataModelAddr);
                
                // ���� ������ ������ ��ȿ���� Ȯ��
                if (!comms->is_connected()) {
                    printf("[!] ����̹� ������ ���������ϴ�. �翬�� �õ� ��...\n");
                    comms = std::make_unique<communication>();
                    
                    if (!comms->is_connected()) {
                        printf("[!] ����̹��� �翬�� ����. �����մϴ�.\n");
                        sleep_ms(5000);
                        exit(0);
                    }
                }
                
                // ���μ��� ��Ȯ�� �� ����
                while (comms->find_process("RobloxPlayerBeta.exe") == 0) {
                    printf("[*] Roblox ���μ��� ��� ��...\n");
                    sleep_ms(500);
                }
                
                if (!comms->v_attach(comms->process_id)) {
                    printf("[!] ���μ��� ���� ����. �����մϴ�.\n");
                    sleep_ms(5000);
                    exit(0);
                }
                
                // �̹��� �ּ� ��Ȯ��
                if (comms->find_image() == 0) {
                    printf("[!] ���̽� �ּҸ� ã�� �� �����ϴ�. �����մϴ�.\n");
                    sleep_ms(5000);
                    exit(0);
                }
                
                // DataModel ������Ʈ
                dataModel = std::move(NewDataModel);
                printf("[+] DataModel �ּ� ������Ʈ: 0x%llx\n", dataModel->getAddress());
                
                // VisualEngine ������Ʈ
                visualEngine = std::make_unique<RBX::VisualEngine>(
                    comms->read<u64>(comms->image_address + Offsets::VisualEngine)
                );
                printf("[+] VisualEngine �ּ� ������Ʈ: 0x%llx\n", visualEngine->getAddress());
                
                // ĳ�̵� �ּ� ������Ʈ
                Cache::DataModel = dataModel->getAddress();
                Cache::VisualEngine = visualEngine->getAddress();
                DataModelOld = Cache::DataModel;
                VisualEngineOld = Cache::VisualEngine;
            }
        }
        catch (const std::exception& e) {
            printf("[!] �ּ� Ȯ�� �� ���� �߻�: %s\n", e.what());
        }
        
        sleep_ms(100);
    }
}
