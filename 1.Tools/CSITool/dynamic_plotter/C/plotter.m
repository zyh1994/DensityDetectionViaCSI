function read_bf_socket()
    while 1
        %% Connect to a TCP Server
        server_ip = '127.0.0.1'; % Replace with the server IP address
        port = 8090;
        tcp_client = tcpip(server_ip, port, 'NetworkRole', 'client');
        tcp_client.InputBufferSize = 1024;
        tcp_client.Timeout = 15;
        fprintf('Connecting to %s on port %d\n', server_ip, port);
        fopen(tcp_client);
        fprintf('Connected to %s\n', tcp_client.RemoteHost);

        %% Set plot parameters
        clf;
        axis([1, 30, -10, 30]);
        time = 0;
        magnitude = zeros(30, 1);

        [~, DATESTR] = version();
        if datenum(DATESTR) > datenum('February 11, 2014')
            plot_handle = plot(time, magnitude, 'MarkerSize', 5);
        else
            plot_handle = plot(time, magnitude, 'EraseMode', 'Xor', 'MarkerSize', 5);
        end

        xlabel('Subcarrier index');
        ylabel('SNR (dB)');

        %% Initialize variables
        csi_entry = [];
        plot_index = -1;              % The index of the plots which need shadowing
        broken_perm = 0;              % Flag marking whether we've encountered a broken CSI yet
        triangle = [1 3 6];           % What perm should sum to for 1, 2, 3 antennas

        %% Process all entries in socket
        while 1
            % Read size and code from the received packets
            s = warning('error', 'instrument:fread:unsuccessfulRead');
            try
                field_len = fread(tcp_client, 1, 'uint16');
            catch
                warning(s);
                disp('Timeout, please restart the client and connect again.');
                break;
            end

            code = fread(tcp_client, 1);

            % If unhandled code, skip (seek over) the record and continue
            if (code == 187) % get beamforming or phy data
                bytes = fread(tcp_client, field_len - 1, 'uint8');
                bytes = uint8(bytes);
                if (length(bytes) ~= field_len - 1)
                    fclose(tcp_client);
                    return;
                end
            elseif field_len <= tcp_client.InputBufferSize  % skip all other info
                fread(tcp_client, field_len - 1, 'uint8');
                continue;
            else
                continue;
            end

            if (code == 187) % Beamforming matrix -- output a record
                csi_entry = read_bfee(bytes);

                perm = csi_entry.perm;
                Nrx = csi_entry.Nrx;
                if Nrx > 1 % No permuting needed for only 1 antenna
                    if sum(perm) ~= triangle(Nrx) % matrix does not contain default values
                        if broken_perm == 0
                            broken_perm = 1;
                            fprintf('WARN ONCE: Found CSI (%s) with Nrx=%d and invalid perm=[%s]\n', filename, Nrx, int2str(perm));
                        end
                    else
                        csi_entry.csi(:, perm(1:Nrx), :) = csi_entry.csi(:, 1:Nrx, :);
                    end
                end
            end

            plot_index = mod(plot_index + 1, 10);
            csi = get_scaled_csi(csi_entry); % CSI data
            % You can use the CSI data here.

            % This plot will show graphics about recent 10 csi packets
            set(plot_handle(plot_index * 3 + 1), 'XData', [1:30], 'YData', db(abs(squeeze(csi(1, 1, :)).')), 'color', 'b', 'linestyle', '-');
            if Nrx > 1
                set(plot_handle(plot_index * 3 + 2), 'XData', [1:30], 'YData', db(abs(squeeze(csi(1, 2, :)).')), 'color', 'g', 'linestyle', '-');
            end
            if Nrx > 2
                set(plot_handle(plot_index * 3 + 3), 'XData', [1:30], 'YData', db(abs(squeeze(csi(1, 3, :)).')), 'color', 'r', 'linestyle', '-');
            end
            axis([1, 30, -10, 40]);
            drawnow;

            csi_entry = [];
        end
        
        %% Close connection
        fclose(tcp_client);
        delete(tcp_client);
    end
end
