# Graph Report - ServerSRC\tools\phoenix_updates  (2026-06-18)

## Corpus Check
- 3 files · ~1,767 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 21 nodes · 21 edges · 6 communities
- Extraction: 100% EXTRACTED · 0% INFERRED · 0% AMBIGUOUS
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `47e11eac`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- [[_COMMUNITY_Community 0|Community 0]]
- [[_COMMUNITY_Community 1|Community 1]]
- [[_COMMUNITY_Community 2|Community 2]]
- [[_COMMUNITY_Community 3|Community 3]]

## God Nodes (most connected - your core abstractions)
1. `Publish` - 7 edges
2. `Project Phoenix Update Endpoint` - 6 edges
3. `Kontrol` - 4 edges
4. `Kurulum` - 3 edges
5. `code:bash (sudo ./install_update_endpoint.sh \)` - 2 edges
6. `code:bash (sudo ./publish_update_release.sh \)` - 2 edges
7. `code:bash (export MYSQL_PWD='DB_PASSWORD')` - 2 edges
8. `Mimari` - 1 edges
9. `code:bash (cd /home/phoenix/phoenix/ServerSRC/tools/phoenix_updates)` - 1 edges
10. `code:powershell (python "F:\Project Phoenix\tools\phoenix_updater\generate_ma)` - 1 edges

## Surprising Connections (you probably didn't know these)
- None detected - all connections are within the same source files.

## Communities (6 total, 0 thin omitted)

### Community 0 - "Community 0"
Cohesion: 0.50
Nodes (4): code:powershell (python "F:\Project Phoenix\tools\phoenix_updater\generate_ma), code:bash (rsync -avz "F:/Project Phoenix/update_publish/" root@89.252.), code:bash (cd /home/phoenix/phoenix/ServerSRC/tools/phoenix_updates), Publish

### Community 1 - "Community 1"
Cohesion: 0.50
Nodes (3): Guvenlik Notlari, Mimari, Project Phoenix Update Endpoint

### Community 2 - "Community 2"
Cohesion: 0.67
Nodes (3): code:bash (cd /home/phoenix/phoenix/ServerSRC/tools/phoenix_updates), code:bash (sudo ./install_update_endpoint.sh \), Kurulum

### Community 3 - "Community 3"
Cohesion: 0.50
Nodes (4): code:bash (sudo ./publish_update_release.sh \), code:bash (export MYSQL_PWD='DB_PASSWORD'), code:bash (curl -I http://127.0.0.1/updates/manifest.json), Kontrol

## Knowledge Gaps
- **7 isolated node(s):** `Mimari`, `code:bash (cd /home/phoenix/phoenix/ServerSRC/tools/phoenix_updates)`, `code:powershell (python "F:\Project Phoenix\tools\phoenix_updater\generate_ma)`, `code:bash (rsync -avz "F:/Project Phoenix/update_publish/" root@89.252.)`, `code:bash (cd /home/phoenix/phoenix/ServerSRC/tools/phoenix_updates)` (+2 more)
  These have ≤1 connection - possible missing edges or undocumented components.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Project Phoenix Update Endpoint` connect `Community 1` to `Community 0`, `Community 2`, `Community 3`?**
  _High betweenness centrality (0.265) - this node is a cross-community bridge._
- **Why does `Publish` connect `Community 0` to `Community 1`, `Community 2`, `Community 3`?**
  _High betweenness centrality (0.256) - this node is a cross-community bridge._
- **Why does `Kontrol` connect `Community 3` to `Community 1`?**
  _High betweenness centrality (0.099) - this node is a cross-community bridge._
- **What connects `Mimari`, `code:bash (cd /home/phoenix/phoenix/ServerSRC/tools/phoenix_updates)`, `code:powershell (python "F:\Project Phoenix\tools\phoenix_updater\generate_ma)` to the rest of the system?**
  _7 weakly-connected nodes found - possible documentation gaps or missing edges._