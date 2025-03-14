# Midas
Midas is a trading platform simulation developed as a personal project to improve my C++ programming skills and enhance my understanding of financial principles. Designed as an experimental playground, it implements multiple order types, live market metrics, and a basic risk management framework, all integrated through a price–time priority matching engine. Individuals can place orders that are automatically traded, while the system tracks real-time performance metrics such as trade outcomes, open positions, and profit/loss. Ultimately, the project serves as demonstration of core trading mechanics and finance fundamentals in a self-contained environment.

## Features
- Supports Limit, Market, Immediate-Or-Cancel, Fill-Or-Kill, Iceberg, Stop, Stop Limit, and Trailing Stop orders.
- Calculates key market metrics, including current price, volatility, bid-ask spread, volume, and trade statistics
- Restricts six primary risk parameters, with limits configurable at both a global level or individually per trader.
- Allocates traders a random inventory and tracks performance metrics, including win rate, P/L, and drawdown.
- Executes trades according to price–time priority, ensuring that the most competitive orders are filled first.

## Technical Implementation

### Order Matching
Active orders are maintained in two data structures that share pointers to the same order objects. A priority queue sorts the orders by price and time so that the best available bids and asks are always at the top, while an unordered map allows fast lookups by unique order IDs. Because both the queue and the map store shared pointers, any update to an order is immediately visible in both structures. This approach avoids unnecessary copying of order objects and simplifies memory management, ensuring that all components work with a single, consistent representation of each order.

### Modular Service Organisation

-   **ActiveOrderService** – Handles orders that are immediately active on the order book.
-   **ConditionalOrderService** – Manages orders that only trigger when specific market conditions are met.
-   **TradeService** – Processes trades by matching orders and recording transaction details.
-   **RiskService** – Enforces risk limits by checking orders and trader performance against six adjustable thresholds, including maximum order size, open position, and risk per order.
-   **TraderService** – Manages trader information, including inventory and performance metrics.
-   **MarketService** – Keeps current market data up to date, such as price and volatility.

### Event Queue System
An event logger collects events (such as order additions, modifications, cancellations, and trade executions and risk limit updates) in a simple queue. A separate thread processes these queued events, updating connected clients via WebSocket. This event-driven approach decouples the core trading logic from client updates, ensuring that order processing is not blocked by communication tasks, and allows the platform to push notifications instead of relying on persistent polling by the clients.

### Database and ORM
The database utilises SQLite and stores data in two tables, orders and trades. When the server starts, both tables are fully loaded into memory. This initial read is the only time data is retrieved from the database, with all subsequent interactions performed as writes. The SQLite database is stored in a .db file, and its path is passed as a command-line argument when the program is run. A basic ORM is implemented using custom mapping and repository classes. The ORM generates SQL statements (for inserting, updating, and selecting records) and maps between C++ objects and the database tables.

This implementation is intentionally straightforward and designed for learning rather than production use.

## Architecture and Components
### Order Types
The platform implements a variety of order types:
-   **Limit and Market Orders** (standard buy and sell orders)
-   **IOC and FOK Orders** (both function as limit orders with specific execution conditions)
-   **Iceberg Orders** (large orders that are partially hidden behind a smaller displayed quantity)
-   **Stop, Stop Limit, and Trailing Stop Orders** (allow for conditional execution based on market movement)

### Risk Management
Each order is checked against preset risk limits. Orders that exceed these limits are rejected. Limits can be set both globally and for individual traders. The following risk checks are performed:
-   **Maximum Order Size** – The maximum quantity that can be traded in a single order.
-   **Open Position** – The maximum number of units a trader can hold at any time.
-   **Daily Loss** – The maximum net loss a trader can incur in a single day.
-   **Orders Per Minute** – The maximum number of orders a trader can place per minute.
-   **Risk Per Trade** – The maximum percentage of a trader's inventory that can be risked on a single trade.
-   **Drawdown** – The maximum percentage a trader's account can decrease from its peak value.

### Matching Engine
The matching engine selects the appropriate matching strategy based on the order type:
-   For **normal** orders, the engine examines the opposing order queue and matches orders based on price and time priority; if an order is only partially filled, it updates the remaining quantity and re-queues the order.
-   For **IOC** orders, the engine applies the normal matching process and then immediately cancels any unfilled quantity, ensuring that only the matched part is executed.
-   For **FOK** orders, the engine first checks the entire opposing queue to determine if there is sufficient quantity available to fill the order completely; if not, the order is cancelled in its entirety without any partial fills.
-   **Iceberg** orders are processed like normal orders, but with an extra step: once the visible portion (display size) is fully executed, the engine replenishes it with the hidden quantity (if available) until the entire order is filled.
-   Throughout the matching process, trade executions and order status changes (such as cancellations or partial fills) are logged, and the engine updates the order book accordingly.

### Trader Management
Traders are represented by unique IDs and assigned an initial inventory. They can track:

-   Total holdings and open orders.
-   Win rate and total closed trades.
-   Realised and unrealised profit/loss.
-   Average entry and exit prices.
-   Maximum drawdown (peak-to-trough decline).

For the sake of simplicity, there is no dedicated trader creation logic. Instead, trader IDs are specified as a URL parameter (for example, `http://localhost:<port>/<traderId>`). When the system encounters a trader ID for the first time, the trader service creates a new trader object and assigns it a random inventory. These trader objects live only in memory for as long as the server runs, and they are not persisted between sessions.

## Client Application
The client is built with React and Tailwind CSS. It offers:

-   Live market data and trade history.
-   Forms to place new orders and update risk limits.
-   Views for monitoring trader performance.

![Midas Example UI](https://i.imgur.com/mj3qpCl.jpeg)

## Project Structure

```
order-book-simulation
├── platform
│ ├── CMakeLists.txt
│ ├── include         # Header files (core logic, models, services, API, etc.)
│ ├── src             # Implementation files
│ ├── test            # Unit tests and mocks (using GTest)
│ └── scripts         # Build, run, and test scripts
├── README.md
└── client            # Frontend application (React, Tailwind CSS, Vite, etc.)
    ├── package.json
    ├── src
    │ ├── App.tsx
    │ ├── components  # UI components, forms, and view
    │ ├── hooks       # Custom React hooks
    │ └── lib         # Utility functions and type definitions
    └── ... (other configuration files)
```

## Build and Test Instructions

### Prerequisites

-   CMake (version 3.10 or higher)
-   A C++23 compiler
-   SQLite3 and Crow libraries
-   GTest (Google Test Framework)
-   Node.js (v22 or higher) and npm

### Building the Platform

1. To build the project, navigate to the `platform` directory and run:

```sh
./scripts/build.sh
```

2. Start the platform (on port 8080) by running:

```sh
./scripts/run.sh
```

3. Run tests with:

```sh
./scripts/test.sh
```

### Building the Client

1. To install dependencies, navigate to the `client` directory and run:
```sh
npm install
```

2. Start the client application by running:

```sh
npm run dev
```

## API Endpoints

### Orders
  -   POST /orders – Create a new order.
  -   GET /orders – Retrieve active and conditional orders.
  -   DELETE /orders/:orderId – Cancel an order.
### Trades
  -   GET /trades – Retrieve trade history.
### Traders
  -   GET /traders/:traderId – Get trader performance metrics.
### Market
  -   GET /market – Get current market data.
### Risk
  -   PUT /risk – Update risk limits (global or per trader).
  -   GET /risk – Retrieve current risk limits (traderId specified as query parameter).
