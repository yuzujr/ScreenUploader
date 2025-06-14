// 命令系统模块
// 处理向客户端发送命令和配置管理

import { selectedClient } from './state.js';
import { showToast } from '../../toast/toast.js';

/**
 * 向客户端发送命令
 * @param {object} command - 要发送的命令对象
 * @param {boolean} showToastMessage - 是否显示toast消息（默认true）
 * @returns {Promise<object|null>} 返回响应结果，如果失败返回null
 */
export async function sendCommand(command, showToastMessage = true) {
    if (!selectedClient) {
        if (showToastMessage) {
            showToast('请先选择客户端');
        }
        return null;
    }

    try {
        const res = await fetch(`/web/command/${selectedClient}`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(command)
        });

        const result = await res.json();

        if (showToastMessage) {
            showToast(result.success ? '命令发送成功' : `命令发送失败: ${result.message}`);
        }

        return result;
    } catch (error) {
        if (showToastMessage) {
            showToast(`发送命令失败: ${error.message}`);
        }
        return null;
    }
}

/**
 * 加载客户端配置
 * @param {string} clientAlias - 客户端别名
 */
export async function loadClientConfig(clientAlias) {
    try {
        const res = await fetch(`/web/config/${clientAlias}`);
        if (!res.ok) {
            console.warn('获取配置失败');
            console.warn(await res.text());
            return;
        }
        const config = await res.json();

        document.getElementById('serverUrl').value = config.server_url || '';
        document.getElementById('wsUrl').value = config.ws_url || '';
        document.getElementById('intervalSeconds').value = config.interval_seconds ?? '';
        document.getElementById('maxRetries').value = config.max_retries ?? '';
        document.getElementById('retryDelayMs').value = config.retry_delay_ms ?? '';
        document.getElementById('addToStartup').checked = !!config.add_to_startup;
    } catch (err) {
        console.error('加载客户端配置出错:', err);
    }
}

/**
 * 更新客户端配置
 */
export async function updateClientConfig() {
    if (!selectedClient) {
        showToast('请先选择客户端');
        return;
    }

    const serverUrl = document.getElementById('serverUrl').value;
    const wsUrl = document.getElementById('wsUrl').value;
    const intervalSeconds = parseInt(document.getElementById('intervalSeconds').value);
    const maxRetries = parseInt(document.getElementById('maxRetries').value);
    const retryDelayMs = parseInt(document.getElementById('retryDelayMs').value);
    const addToStartup = document.getElementById('addToStartup').checked;

    if (!serverUrl || !wsUrl || isNaN(intervalSeconds) || isNaN(maxRetries) || isNaN(retryDelayMs)) {
        showToast('请填写正确的配置参数');
        return;
    }

    const newConfig = {
        api: {
            server_url: serverUrl,
            ws_url: wsUrl,
            interval_seconds: intervalSeconds,
            max_retries: maxRetries,
            retry_delay_ms: retryDelayMs,
            add_to_startup: addToStartup
        }
    };

    const cmd = {
        type: "update_config",
        data: newConfig
    };

    await sendCommand(cmd);
}
