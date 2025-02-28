/*
MIT License

Copyright (c) 2019 Vladislav Gusak

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "CChristofidesSolver.hpp"

#include <algorithm>
#include <stack>
#include <iostream>
#include <map>
#include <numeric>

using namespace TSP;

namespace {
    struct Edge
    {
        size_t from;
        size_t to;
        double distance;

        bool operator==(const Edge& edge)
        {
            return (edge.from == from) && (to == edge.to) && (distance == edge.distance);
        }
    };

    Matrix createCompleteGraph(const std::vector<Edge>& edges)
    {
        size_t max_idx{0};
        for(const auto& edge : edges)
        {
            max_idx = std::max({edge.from, edge.to, max_idx});
        }

        Matrix result_matrix(max_idx+1);
        for (auto& row : result_matrix)
        {
            row.resize(result_matrix.size(), 0);
        }

        for (const auto& edge : edges)
        {
            result_matrix[edge.from][edge.to] = edge.distance;
            result_matrix[edge.to][edge.from] = edge.distance;
        }

        return result_matrix;
    }

    std::vector<std::vector<size_t>> findMinimumWeightMatching(const Matrix& matrix, 
                                                std::vector<size_t> odd_vertices)
    {
        std::vector<std::vector<size_t>> minimum_weighted_matching(matrix.size());
        size_t vertex;
        while (!odd_vertices.empty())
        {
            vertex = odd_vertices.front();
            auto isCloser = [vertex, &matrix](size_t lhs, size_t rhs) 
            {
                return matrix[vertex][lhs] != 0 &&
                       matrix[vertex][lhs] < matrix[vertex][rhs];
            };
            auto closest_element = std::min_element(odd_vertices.begin()+1,
                                                    odd_vertices.end(),
                                                    isCloser);

            minimum_weighted_matching[*closest_element].push_back(vertex);
            minimum_weighted_matching[vertex].push_back(*closest_element);
            odd_vertices.erase(closest_element);
            odd_vertices.erase(odd_vertices.begin());
        }

        return minimum_weighted_matching;
    }

    std::vector<Edge> extractEdges(const Matrix& matrix)
    {
        std::vector<Edge> edges;

        for (size_t i = 0; i < matrix.size(); ++i)
        {
            for (size_t j = 0; j < matrix[i].size(); ++j)
            {
                if (matrix[i][j] != 0)
                {
                    edges.push_back(Edge{i, j, matrix[i][j]});
                }
            }
        }

        return edges;
    }

    std::vector<std::vector<size_t>> convertEdges(const std::vector<Edge>& edges)
    {
        size_t max_idx{0};
        auto setMaxIdx = [&max_idx](const Edge& edge){max_idx = std::max({edge.from, edge.to, max_idx});};
        std::for_each(edges.begin(), edges.end(), setMaxIdx);

        std::vector<std::vector<size_t>> result(max_idx+1, std::vector<size_t>(max_idx+1, 0));

        for (const auto& edge : edges)
        {
            ++result[edge.from][edge.to];
        }

        return result;
    }

    std::vector<size_t> getEulerPath(const std::vector<Edge>& edges, size_t root)
    {
        std::vector<size_t> path;
        auto vertices_connections = convertEdges(edges);

        std::stack<size_t> stack;
        stack.push(0);

        while (!stack.empty())
        {
            size_t i = 0;
            size_t vertex = stack.top();

            for (; i < vertices_connections[vertex].size(); ++i)
            {
                if (vertices_connections[vertex][i] != 0)
                {
                    break;
                }
            }

            if (i == vertices_connections[vertex].size())
            {
                path.push_back(vertex);
                stack.pop();
            }
            else
            {
                --vertices_connections[vertex][i];
                --vertices_connections[i][vertex];
                stack.push(i);
            }
        }

        return path;
    }

    std::vector<size_t> getHamiltonPath(std::vector<size_t> euler_path)
    {
        std::map<size_t, bool> visited;
        for (size_t element : euler_path)
        {
            visited[element] = false;
        }

        int root = euler_path.front();
        
        auto current = euler_path.begin();
        auto next = euler_path.begin()+1;

        visited[root] = true;

        while (next != euler_path.end())
        {
            if (!visited[*next])
            {
                current = next;
                visited[*current] = true;
                next = current+1;
            }
            else
            {
                next = euler_path.erase(next);
            }
        }

        return euler_path;
    }

    namespace Kruskal {
        Matrix getMinSpanningTree(const Matrix& matrix)
        {
            std::vector<Edge> edges, min_edges;
            for (size_t i = 0; i < matrix.size(); ++i)
            {
                for (size_t j = i; j < matrix[i].size(); ++j)
                {
                    edges.push_back(Edge{i, j, matrix[i][j]});
                }
            }

            std::sort(edges.begin(), edges.end(),
                [](const Edge& lhs, const Edge& rhs) {return lhs.distance < rhs.distance;});
            std::vector<size_t> components(matrix.size());
            std::iota(components.begin(), components.end(), 0); 

            for (const auto& edge : edges)
            {
                if (components[edge.from] != components[edge.to])
                {
                    size_t new_component = components[edge.from];
                    size_t old_component = components[edge.to];
                    min_edges.push_back(edge);

                    for (size_t& component : components)
                    {
                        component = (component == old_component ? new_component 
                                                                : component);
                    }
                }
            }

            return createCompleteGraph(min_edges);
        }
    }
}

size_t swapVerticesInPath(std::vector<size_t>& path, const Matrix& matrix, size_t a, size_t b, size_t old_cost)
{
    if (a > b)
    {
        std::swap(a, b);
    }


    if (a == 0)
    {
        old_cost -= matrix[path[b-1]][path[b]] + matrix[path[a]][path[a+1]];
        old_cost += matrix[path[b-1]][path[a]] + matrix[path[b]][path[a+1]];
    }
    else
    {
        old_cost -= matrix[path[b-1]][path[b]] + matrix[path[a]][path[a+1]] + matrix[path[a-1]][path[a]];
        old_cost += matrix[path[b-1]][path[a]] + matrix[path[b]][path[a+1]] + matrix[path[a-1]][path[b]];
    }

    if (b != path.size()-1)
    {
        old_cost -= matrix[path[b]][path[b-1]];
        old_cost += matrix[path[a]][path[b-1]];
    }

    std::swap(path[a], path[b]);

    return old_cost;
}

std::vector<size_t> CChristofidesSolver::solve(const Matrix& matrix, size_t root)
{
    std::vector<size_t> min_sequence;

    Matrix min_matrix = Kruskal::getMinSpanningTree(matrix);

    std::vector<size_t> odd_degree_vertices;
    auto isNotZero = [](size_t value) {return value != 0;};

    for (size_t i = 0; i < min_matrix.size(); ++i)
    {
        size_t degree = std::count_if(min_matrix[i].begin(), min_matrix[i].end(), isNotZero);
        if (degree % 2 != 0)
        {
            odd_degree_vertices.push_back(i);
        }
    }

    auto minimum_weighted_matching = findMinimumWeightMatching(matrix, odd_degree_vertices);

    std::vector<Edge> edges = extractEdges(min_matrix);

    for (size_t i = 0; i < minimum_weighted_matching.size(); ++i)
    {
        for (size_t j = 0; j < minimum_weighted_matching[i].size(); ++j)
        {
            size_t to = minimum_weighted_matching[i][j];

            Edge e1{to, i, matrix[i][to]};
            Edge e2{i, to, matrix[to][i]};
            if (std::find(edges.begin(), edges.end(), e1) == edges.end())
            {
                edges.push_back(e1);
            }
            if (std::find(edges.begin(), edges.end(), e2) == edges.end())
            {
                edges.push_back(e2);
            }
        }
    }   

    std::vector<size_t> euler_path = getEulerPath(edges, root);
    min_sequence = getHamiltonPath(euler_path);

    return min_sequence;
}