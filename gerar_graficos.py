import matplotlib.pyplot as plt
import re
import os

def extract_rtt_from_log(filename):
    sent_times = {}
    received_times = {}

    try:
        with open(filename, 'r') as f:
            for line in f:
                if "client sent" in line:
                    match = re.search(r'At time \+(\S+)s client sent', line)
                    if match:
                        time = float(match.group(1))

                        packet_num = 1
                        if sent_times:
                            packet_num = max(sent_times.keys()) + 1
                        sent_times[packet_num] = time

                elif "client received" in line:
                    match = re.search(r'At time \+(\S+)s client received', line)
                    if match:
                        time = float(match.group(1))

                        packet_num = 1
                        if received_times:
                            packet_num = max(received_times.keys()) + 1
                        received_times[packet_num] = time
    except FileNotFoundError:
        print(f"Erro: Arquivo '{filename}' não encontrado.")
        return [], []

    rtts = []
    packet_numbers = sorted(sent_times.keys())
    temp_received_keys = sorted(received_times.keys())

    for i in range(min(len(packet_numbers), len(temp_received_keys))):
        sent_key = packet_numbers[i]
        rcvd_key = temp_received_keys[i]
        rtt = (received_times[rcvd_key] - sent_times[sent_key]) * 1000
        rtts.append(rtt)

    return packet_numbers[:len(rtts)], rtts

home_dir = os.path.expanduser('~')
desktop_path = os.path.join(home_dir, 'Desktop')

original_packets, original_rtts = extract_rtt_from_log('saida_original.txt')
modificada_packets, modificada_rtts = extract_rtt_from_log('saida_modificada.txt')

if original_packets and modificada_packets:
    plt.figure(figsize=(10, 6))
    plt.plot(original_packets, original_rtts, marker='o', linestyle='-', label='Topologia Original')
    plt.plot(modificada_packets, modificada_rtts, marker='x', linestyle='--', label='Topologia Modificada')
    plt.title('Comparativo de RTT (Parte 2)')
    plt.xlabel('Número do Pacote')
    plt.ylabel('Atraso (ms)')
    plt.grid(True)
    plt.legend()
    plt.xticks(original_packets)

    output_path_part2 = os.path.join(desktop_path, 'grafico_parte2.png')
    plt.savefig(output_path_part2)
    print(f"Gráfico '{output_path_part2}' gerado na Área de Trabalho.")

wifi_packets, wifi_rtts = extract_rtt_from_log('saida_parte3.txt')

if wifi_packets:
    plt.figure(figsize=(10, 6))
    plt.plot(wifi_packets, wifi_rtts, marker='s', linestyle='-', color='green', label='Atraso RTT WiFi')
    plt.title('Variação do RTT em Rede WiFi (Parte 3)')
    plt.xlabel('Número do Pacote')
    plt.ylabel('Atraso (ms)')
    plt.grid(True)
    plt.legend()
    plt.xticks(wifi_packets)

    output_path_part3 = os.path.join(desktop_path, 'grafico_parte3.png
