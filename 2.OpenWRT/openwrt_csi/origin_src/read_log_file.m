% 开始定义read_log_file函数，该函数接受一个参数：日志文件的文件名
function ret = read_log_file(filename)

% 打开指定文件名的文件，'rb'表示以二进制形式读取
f = fopen(filename, 'rb');

% 如果文件打开失败，则输出错误信息并返回
if (f < 0)
    error('couldn''t open file %s', filename);
    return;
end

% 将文件指针移动到文件的末尾，获取文件的长度
status = fseek(f, 0, 'eof');
if status ~= 0
    [msg, errno] = ferror(f);
    error('Error %d seeking: %s', errno, msg);
    fclose(f);
    return;
end
len = ftell(f); % 获取文件的长度（字节数）
fprintf('file length is:%d\n',len);

% 将文件指针移动回文件的开头，开始读取文件
status = fseek(f, 0, 'bof');
if status ~= 0
    [msg, errno] = ferror(f);
    error('Error %d seeking: %s', errno, msg);
    fclose(f);
    return;
end

% 初始化输出结果的单元格数组，预计每420字节为一个单元格
ret = cell(ceil(len / 420),1);
cur = 0; % 当前读取的位置
count = 0; % 已读取的数据块的数量

% 设置数据的字节序格式为大端
endian_format = 'ieee-be';

% 循环读取文件，直到文件的末尾
while cur < (len - 4)

    % 读取当前块的长度
    field_len = fread(f, 1, 'uint16', 0, endian_format);
	cur = cur + 2;
    fprintf('Block length is:%d\n',field_len);

    % 如果当前块的长度大于剩余的文件长度，则跳出循环
	if (cur + field_len) > len
   		break;
    end
    
    % 读取并存储时间戳等各种参数，这些参数被存储在csi_matrix中
    % 每读取一个参数，cur都会增加对应的长度
    % 在读取每个参数后，都会打印出该参数的值

    % 具体读取内容如下：
    % 1. 时间戳
    timestamp = fread(f, 1, 'uint64', 0, [endian_format '.l64']);
	csi_matrix.timestamp = timestamp;
	cur = cur + 8;
	fprintf('timestamp is %d\n',timestamp);

    % 2. CSI长度
    csi_len = fread(f, 1, 'uint16', 0, endian_format);
	csi_matrix.csi_len = csi_len;
	cur = cur + 2;
    fprintf('csi_len is %d\n',csi_len);

    % 3. 传输通道
    tx_channel = fread(f, 1, 'uint16', 0, endian_format);
	csi_matrix.channel = tx_channel;
	cur = cur + 2;
    fprintf('channel is %d\n',tx_channel);

    % 4. 错误信息
    err_info = fread(f, 1,'uint8=>int');
    csi_matrix.err_info = err_info;
    fprintf('err_info is %d\n',err_info);
    cur = cur + 1;

    % 5. 噪声水平
    noise_floor = fread(f, 1, 'uint8=>int');
	csi_matrix.noise_floor = noise_floor;
	cur = cur + 1;
    fprintf('noise_floor is %d\n',noise_floor);

    % 6. 速率
    Rate = fread(f, 1, 'uint8=>int');
	csi_matrix.Rate = Rate;
	cur = cur + 1;
	fprintf('rate is %x\n',Rate);

    % 7. 带宽
    bandWidth = fread(f, 1, 'uint8=>int');
	csi_matrix.bandWidth = bandWidth;
	cur = cur + 1;
	fprintf('bandWidth is %d\n',bandWidth);

    % 8. 音调数量
    num_tones = fread(f, 1, 'uint8=>int');
	csi_matrix.num_tones = num_tones;
	cur = cur + 1;
	fprintf('num_tones is %d  ',num_tones);

    % 9. 发送天线数量
	nr = fread(f, 1, 'uint8=>int');
	csi_matrix.nr = nr;
	cur = cur + 1;
	fprintf('nr is %d  ',nr);

    % 10. 接收天线数量
	nc = fread(f, 1, 'uint8=>int');
	csi_matrix.nc = nc;
	cur = cur + 1;
	fprintf('nc is %d\n',nc);

    % 11. RSSI
	rssi = fread(f, 1, 'uint8=>int');
	csi_matrix.rssi = rssi;
	cur = cur + 1;
	fprintf('rssi is %d\n',rssi);

    % 12. RSSI1
	rssi1 = fread(f, 1, 'uint8=>int');
	csi_matrix.rssi1 = rssi1;
	cur = cur + 1;
	fprintf('rssi1 is %d\n',rssi1);

    % 13. RSSI2
	rssi2 = fread(f, 1, 'uint8=>int');
	csi_matrix.rssi2 = rssi2;
	cur = cur + 1;
	fprintf('rssi2 is %d\n',rssi2);

    % 14. RSSI3
	rssi3 = fread(f, 1, 'uint8=>int');
	csi_matrix.rssi3 = rssi3;
	cur = cur + 1;
	fprintf('rssi3 is %d\n',rssi3);

    % 15. 有效载荷长度
    payload_len = fread(f, 1, 'uint16', 0, endian_format);
	csi_matrix.payload_len = payload_len;
	cur = cur + 2;
    fprintf('payload length: %d\n',payload_len);	

    % 如果csi_len > 0，读取CSI数据并将其添加到csi_matrix中
    if csi_len > 0
        csi_buf = fread(f, csi_len, 'uint8=>uint8');
	    csi = read_csi(csi_buf, nr, nc, num_tones);
    	cur = cur + csi_len;
	    csi_matrix.csi = csi;
    else
        csi_matrix.csi = 0;
    end

    % 如果payload_len > 0，读取有效载荷并将其添加到csi_matrix中
    if payload_len > 0
        data_buf = fread(f, payload_len, 'uint8=>uint8');	    
    	cur = cur + payload_len;
	    csi_matrix.payload = data_buf;
    else
        csi_matrix.payload = 0;
    end

    % 每读取完一个数据块，都将csi_matrix添加到结果数组中，并增加计数器
    if (cur + 420 > len)
        break;
    end
    count = count + 1;
    ret{count} = csi_matrix;
end

% 如果读取了多于一个数据块，则输出所有读取的数据块
% 如果只读取了一个数据块，则输出该数据块
if (count >1)
	ret = ret(1:(count-1));
else
	ret = ret(1);
end

% 关闭文件
fclose(f);
% 结束函数定义
%end
