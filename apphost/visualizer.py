import yaml
import networkx as nx
import matplotlib.pyplot as plt
import argparse
import os
from typing import Dict, List, Any, Tuple, Optional
import numpy as np


class ServiceGraphException(Exception):
    pass


class YamlParser:

    @staticmethod
    def load_yaml(file_path: str) -> Dict[str, Any]:
        """Загружает YAML файл и возвращает его содержимое."""
        if not os.path.exists(file_path):
            raise FileNotFoundError(f"Файл не найден: {file_path}")

        with open(file_path, 'r', encoding='utf-8') as file:
            return yaml.safe_load(file)


class ServiceDependencyGraph:
    def __init__(self, yaml_data: Dict[str, Any]):
        """Инициализирует граф из YAML-данных."""
        self.yaml_data = yaml_data
        self.graph = nx.DiGraph()
        self.service_info = {}
        self._build_graph()

    def _build_graph(self) -> None:
        """Строит граф зависимостей сервисов на основе YAML-данных."""
        if 'flows' not in self.yaml_data:
            raise ServiceGraphException("YAML файл не содержит раздел 'flows'")

        entrypoint = self.yaml_data.get('entrypoint')
        if not entrypoint or entrypoint not in self.yaml_data['flows']:
            raise ServiceGraphException(f"Точка входа '{entrypoint}' не найдена в flows")

        for flow_name, flow_data in self.yaml_data['flows'].items():
            if 'nodes' not in flow_data:
                continue

            node_id_to_service = {}

            for node in flow_data['nodes']:
                node_id = node['id']
                service_name = node['call']
                node_id_to_service[node_id] = service_name

                if service_name not in self.service_info:
                    self.service_info[service_name] = {
                        'node_ids': [],
                        'config': {},
                        'flows': []
                    }

                self.service_info[service_name]['node_ids'].append(node_id)

                if 'config' in node:
                    self.service_info[service_name]['config'][node_id] = node['config']

                if flow_name not in self.service_info[service_name]['flows']:
                    self.service_info[service_name]['flows'].append(flow_name)

                if not self.graph.has_node(service_name):
                    self.graph.add_node(service_name)

            for node in flow_data['nodes']:
                service_name = node['call']

                if 'depends_on' in node:
                    for dependency_id in node['depends_on']:
                        if dependency_id in node_id_to_service:
                            dependency_service = node_id_to_service[dependency_id]
                            self.graph.add_edge(dependency_service, service_name)

    def get_graph(self) -> nx.DiGraph:
        """Возвращает граф зависимостей."""
        return self.graph

    def get_service_info(self) -> Dict[str, Dict]:
        """Возвращает информацию о сервисах."""
        return self.service_info


class ServiceGraphVisualizer:
    """Класс для визуализации графа зависимостей сервисов."""

    def __init__(self, graph: nx.DiGraph, service_info: Dict[str, Dict]):
        """Инициализирует визуализатор с графом и информацией о сервисах."""
        self.graph = graph
        self.service_info = service_info

    def visualize(self, output_file: Optional[str] = None) -> None:
        """Визуализирует граф зависимостей сервисов."""
        fig, ax = plt.subplots(figsize=(8, 8))

        pos = nx.shell_layout(self.graph)
        safe_scale = 0.9
        for node in pos:
            pos[node] = (pos[node][0] * safe_scale, pos[node][1] * safe_scale)

        # Цвета узлов в зависимости от количества потоков
        node_colors = []
        for node in self.graph.nodes():
            flows_count = len(self.service_info[node]['flows'])
            if flows_count > 1:
                node_colors.append('lightcoral')
            else:
                node_colors.append('skyblue')

        # Создаем метки для узлов и определяем размеры
        labels = {}
        node_sizes = {}

        for node in self.graph.nodes():
            info = self.service_info[node]
            timeout = None

            for node_id, config in info['config'].items():
                if 'timeout' in config:
                    timeout = config['timeout']

            label = f"{node}"
            if timeout:
                label += f"\n(timeout: {timeout})"

            labels[node] = label

            base_size = 800 + 500 * self.graph.out_degree(node)
            label_size_factor = len(label) * 80
            node_sizes[node] = base_size + label_size_factor

        nodes = nx.draw_networkx_nodes(
            self.graph,
            pos,
            node_size=[node_sizes[node] for node in self.graph.nodes()],
            node_color=node_colors,
            alpha=0.8,
            edgecolors='gray',
            ax=ax
        )

        for src, dst in self.graph.edges():
            src_x, src_y = pos[src]
            dst_x, dst_y = pos[dst]

            dx = dst_x - src_x
            dy = dst_y - src_y
            d = np.sqrt(dx * dx + dy * dy)

            if d == 0:
                continue

            dx, dy = dx / d, dy / d

            radius_src = np.sqrt(node_sizes[src]) / 85
            radius_dst = np.sqrt(node_sizes[dst]) / 85

            adj_factor = 0.14
            start_x = src_x + dx * radius_src * adj_factor
            start_y = src_y + dy * radius_src * adj_factor
            end_x = dst_x - dx * radius_dst * adj_factor
            end_y = dst_y - dy * radius_dst * adj_factor

            arrow = plt.matplotlib.patches.FancyArrowPatch(
                (start_x, start_y),
                (end_x, end_y),
                connectionstyle='arc3,rad=0.08',
                arrowstyle='-|>',
                mutation_scale=15,
                linewidth=1.5,
                color='dimgray',
                alpha=0.8
            )
            ax.add_patch(arrow)

        nx.draw_networkx_labels(
            self.graph,
            pos,
            labels=labels,
            font_size=9,
            font_family='sans-serif',
            font_weight='bold',
            horizontalalignment='center',
            verticalalignment='center',
            bbox=dict(boxstyle="round,pad=0.3", fc="white", ec="gray", alpha=0.7),
            ax=ax
        )

        ax.set_title("Граф зависимостей микросервисов", fontsize=16)

        legend_text = (
            "• Голубой - сервис используется в одном потоке\n"
            "• Красный - сервис используется в нескольких потоках\n"
            "• Размер узла показывает количество зависимых сервисов\n"
            "• Направление стрелки: A → B означает B зависит от A"
        )
        fig.text(
            0.5, 0.01,
            legend_text,
            ha="center",
            fontsize=11,
            bbox={"facecolor":"white", "alpha":0.9, "pad":5}
        )

        margin = 0.2
        ax.set_xlim(-1 - margin, 1 + margin)
        ax.set_ylim(-1 - margin, 1 + margin)

        ax.axis('off')

        fig.tight_layout(pad=1.0)

        if output_file:
            fig.savefig(
                output_file,
                format='png',
                dpi=300,
                bbox_inches='tight',
                pad_inches=0.5
            )
            print(f"Граф сохранен в файл: {output_file}")
        else:
            plt.show()



class ServiceGraphAnalyzer:
    """Класс для анализа графа зависимостей сервисов."""

    def __init__(self, graph: nx.DiGraph, service_info: Dict[str, Dict]):
        """Инициализирует анализатор с графом и информацией о сервисах."""
        self.graph = graph
        self.service_info = service_info

    def print_stats(self) -> None:
        """Выводит статистику по сервисам."""
        print(f"Всего сервисов: {self.graph.number_of_nodes()}")
        print(f"Всего зависимостей: {self.graph.number_of_edges()}")

        # Находим сервисы с наибольшим количеством зависимых от них сервисов
        most_depended_on = sorted(
            self.graph.nodes(),
            key=lambda n: self.graph.out_degree(n),
            reverse=True
        )[:3]

        print("\nСервисы, от которых зависит больше всего других сервисов:")
        for svc in most_depended_on:
            deps = list(self.graph.successors(svc))
            print(f"  - {svc}: {self.graph.out_degree(svc)} зависимых сервисов" +
                  (f" ({', '.join(deps)})" if deps else ""))

        most_dependent = sorted(
            self.graph.nodes(),
            key=lambda n: self.graph.in_degree(n),
            reverse=True
        )[:3]

        print("\nСервисы с наибольшим количеством зависимостей:")
        for svc in most_dependent:
            deps = list(self.graph.predecessors(svc))
            print(f"  - {svc}: зависит от {self.graph.in_degree(svc)} сервисов" +
                  (f" ({', '.join(deps)})" if deps else ""))


class ServiceGraphApp:
    """Основной класс приложения для построения и визуализации графа сервисов."""

    def __init__(self):
        """Инициализирует приложение."""
        self.parser = self._create_parser()

    def _create_parser(self) -> argparse.ArgumentParser:
        """Создает и возвращает парсер аргументов командной строки."""
        parser = argparse.ArgumentParser(
            description='Утилита для построения графа зависимостей микросервисов из YAML-конфигурации'
        )
        parser.add_argument('yaml_file', help='Путь к YAML файлу с описанием сервисов')
        parser.add_argument('-o', '--output', help='Путь для сохранения изображения графа (необязательно)')
        parser.add_argument('-s', '--stats', action='store_true', help='Показать статистику по сервисам')
        return parser

    def run(self) -> int:
        """Запускает приложение с обработкой аргументов командной строки."""
        args = self.parser.parse_args()

        try:
            yaml_data = YamlParser.load_yaml(args.yaml_file)

            dependency_graph = ServiceDependencyGraph(yaml_data)
            graph = dependency_graph.get_graph()
            service_info = dependency_graph.get_service_info()

            if args.stats:
                analyzer = ServiceGraphAnalyzer(graph, service_info)
                analyzer.print_stats()

            visualizer = ServiceGraphVisualizer(graph, service_info)
            visualizer.visualize(args.output)

            return 0

        except Exception as e:
            print(f"Ошибка: {e}")
            return 1


def main():
    """Точка входа в программу."""
    app = ServiceGraphApp()
    return app.run()


if __name__ == "__main__":
    exit(main())
