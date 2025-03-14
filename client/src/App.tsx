import { Toaster } from "sonner";
import { ThemeProvider } from "./components/providers/theme-provider";
import Dashboard from "./pages/Dashboard";
import "./app.css";

function App() {
	return (
		<ThemeProvider defaultTheme="dark" storageKey="vite-ui-theme">
			<Dashboard />
			<Toaster position="bottom-left" />
		</ThemeProvider>
	);
}

export default App;
